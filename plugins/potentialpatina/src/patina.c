/* PotentialPatina (PotentialBreaks): a retro-colour effect for MPC OS standalone devices.
 * Six modules in one insert, each with an on/off switch, and one MAGNITUDE control that scales them all:
 *   NOISE   vinyl crackle + surface hiss / tape hiss / mains hum (50 Hz), mixed in before the rest
 *   WOBBLE  wow and flutter: a modulated delay line (slow wow, fast flutter, random drift)
 *   DRIVE   saturation: tape, tube or cassette
 *   DIGITAL bit depth and sample-rate reduction
 *   TONE    tilt EQ (dark <-> bright)
 *   SPACE   a small Freeverb-style room
 * then dry/wet MIX and OUTPUT. 44.1 kHz, interleaved int16 stereo, 128-frame blocks (wrapper/engine.h). */
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "engine.h"

#define FS 44100.0f
#define PI_F 3.14159265f

/* ---- parameters: same order and keys as vst/params.json ---- */
enum {
    P_MAG, P_MIX, P_OUT,
    P_NOISE_ON, P_NOISE_TYPE, P_NOISE_AMT,
    P_WOB_ON, P_WOB_AMT, P_WOB_RATE,
    P_DRV_ON, P_DRV_TYPE, P_DRV_AMT,
    P_DIG_ON, P_DIG_BITS, P_DIG_RATE,
    P_TONE_ON, P_TONE_TILT,
    P_SPC_ON, P_SPC_AMT, P_SPC_SIZE,
    NP
};
static const struct { const char *key; float def; } PDEF[NP] = {
    {"magnitude", 100}, {"mix", 85}, {"output", 0},
    {"noise_on", 1}, {"noise_type", 0}, {"noise_amt", 19},
    {"wobble_on", 1}, {"wobble_amt", 18}, {"wobble_rate", 30},
    {"drive_on", 1}, {"drive_type", 0}, {"drive_amt", 23},
    {"digital_on", 0}, {"digital_bits", 12}, {"digital_rate", 26},
    {"tone_on", 1}, {"tone_tilt", -12},
    {"space_on", 0}, {"space_amt", 25}, {"space_size", 50},
};

/* ---- small DSP pieces ---- */
static inline float frand(uint32_t *s) {   /* xorshift32 -> -1..1 */
    uint32_t x = *s; x ^= x << 13; x ^= x >> 17; x ^= x << 5; *s = x;
    return (float)(int32_t)x * (1.0f / 2147483648.0f);
}
static inline float urand(uint32_t *s) { return 0.5f * (frand(s) + 1.0f); }   /* 0..1 */
static inline float onepole_k(float hz) { return 1.0f - expf(-2.0f * PI_F * hz / FS); }

#define DRV_COMP 1.0f
#define WOB_LEN 4096            /* delay line for wobble (> 2 x the largest delay) */
#define WOB_BASE 300.0f         /* centre delay, samples (~6.8 ms) */

/* Freeverb tunings (44.1 kHz), stereo spread on the right */
static const int COMB_T[4] = {1116, 1188, 1277, 1356};
static const int AP_T[2] = {556, 441};
#define SPREAD 23
typedef struct { float buf[1356 + SPREAD]; int len, pos; float store; } comb_t;
typedef struct { float buf[556 + SPREAD]; int len, pos; } ap_t;

typedef struct {
    float p[NP];                       /* parameter values in their own units */
    /* smoothed controls */
    float s_mag, s_mix, s_out, s_noise, s_wob, s_wrate, s_drv, s_bits, s_rate, s_tilt, s_spc, s_size;
    float g_noise, g_wob, g_drv, g_dig, g_tone, g_spc;     /* smoothed on/off gains (click-free switching) */
    uint32_t rng[3];
    /* noise */
    float hiss_lp[2], hiss_hp[2], crack_env, crack_sign, pop_env, pop_lp, rumble, hum_ph;
    /* wobble */
    float wbuf[2][WOB_LEN], dbuf[2][WOB_LEN]; int wpos; float wow_ph, flut_ph, drift, drift_target; int drift_count;
    /* drive / cassette filter */
    float cas_lp[2];
    /* digital */
    float hold[2], hold_ph;
    /* tone */
    float tilt_lp[2];
    /* space */
    comb_t comb[2][4]; ap_t ap[2][2];
} patina_t;

static void reset(patina_t *s) {
    memset(s, 0, sizeof *s);
    for (int i = 0; i < NP; i++) s->p[i] = PDEF[i].def;
    s->rng[0] = 0x1234567u; s->rng[1] = 0x89abcdefu; s->rng[2] = 0x2468aceu;
    for (int c = 0; c < 2; c++) {
        for (int i = 0; i < 4; i++) s->comb[c][i].len = COMB_T[i] + (c ? SPREAD : 0);
        for (int i = 0; i < 2; i++) s->ap[c][i].len = AP_T[i] + (c ? SPREAD : 0);
    }
    s->drift_target = 0; s->crack_sign = 1;
    /* start the smoothed values at their targets */
    s->s_mag = s->p[P_MAG] / 100.0f; s->s_mix = s->p[P_MIX] / 100.0f; s->s_out = powf(10.0f, s->p[P_OUT] / 20.0f);
    s->s_bits = 16.0f; s->s_rate = 44.1f; s->s_wrate = s->p[P_WOB_RATE] / 100.0f; s->s_size = s->p[P_SPC_SIZE] / 100.0f;
}

static void *create(const char *data_dir) {
    (void)data_dir;
    patina_t *s = malloc(sizeof *s);
    if (s) reset(s);
    return s;
}
static void destroy(void *inst) { free(inst); }
static void midi(void *inst, const uint8_t *msg, int len) { (void)inst; (void)msg; (void)len; }

static int find(const char *key) {
    for (int i = 0; i < NP; i++) if (!strcmp(key, PDEF[i].key)) return i;
    return -1;
}

static void set_param(void *inst, const char *key, const char *val) {
    patina_t *s = inst;
    if (!strcmp(key, "state")) {   /* "key=value;key=value;..." */
        char tmp[1024]; snprintf(tmp, sizeof tmp, "%s", val);
        for (char *tok = strtok(tmp, ";"); tok; tok = strtok(NULL, ";")) {
            char *eq = strchr(tok, '=');
            if (!eq) continue;
            *eq = 0;
            int i = find(tok);
            if (i >= 0) s->p[i] = (float)atof(eq + 1);
        }
        return;
    }
    int i = find(key);
    if (i >= 0) s->p[i] = (float)atof(val);
}

static int get_param(void *inst, const char *key, char *buf, int buf_len) {
    patina_t *s = inst;
    if (!strcmp(key, "state")) {
        int n = 0;
        for (int i = 0; i < NP && n < buf_len; i++) n += snprintf(buf + n, buf_len - n, "%s=%g;", PDEF[i].key, (double)s->p[i]);
        return n;
    }
    int i = find(key);
    if (i < 0) return 0;
    if (i == P_NOISE_ON || i == P_NOISE_TYPE || i == P_WOB_ON || i == P_DRV_ON || i == P_DRV_TYPE ||
        i == P_DIG_ON || i == P_DIG_BITS || i == P_TONE_ON || i == P_SPC_ON)
        return snprintf(buf, buf_len, "%d", (int)lroundf(s->p[i]));
    return snprintf(buf, buf_len, "%g", (double)s->p[i]);
}

static void render(void *inst, int16_t *out_lr, int frames) {   /* an effect: silence without input */
    (void)inst;
    memset(out_lr, 0, (size_t)frames * 2 * sizeof(int16_t));
}

static inline float smooth(float cur, float target, float k) { return cur + (target - cur) * k; }

/* saturators, all roughly unity gain for small signals */
static inline float sat_tape(float x) { return tanhf(x); }
static inline float sat_tube(float x) {        /* asymmetric: softer on the positive side, even harmonics */
    return x >= 0 ? 1.0f - expf(-x) : -(1.0f - expf(1.25f * x)) / 1.25f;
}

static void process(void *inst, const int16_t *in_lr, int16_t *out_lr, int frames) {
    patina_t *s = inst;
    const float k = 0.25f;   /* per-block smoothing (~3 ms blocks) */
    float mag_t = s->p[P_MAG] / 100.0f;
    s->s_mag = smooth(s->s_mag, mag_t, k);
    float m = s->s_mag;
    s->s_mix = smooth(s->s_mix, s->p[P_MIX] / 100.0f, k);
    s->s_out = smooth(s->s_out, powf(10.0f, s->p[P_OUT] / 20.0f), k);
    s->s_noise = smooth(s->s_noise, s->p[P_NOISE_AMT] / 100.0f * m, k);
    s->s_wob = smooth(s->s_wob, s->p[P_WOB_AMT] / 100.0f * m, k);
    s->s_wrate = smooth(s->s_wrate, s->p[P_WOB_RATE] / 100.0f, k);
    s->s_drv = smooth(s->s_drv, s->p[P_DRV_AMT] / 100.0f * m, k);
    /* digital: magnitude 0 = clean (16 bit, 44.1 kHz), 1 = the knobs' settings */
    float bits_t = 16.0f - (16.0f - s->p[P_DIG_BITS]) * m;
    float rate_t = 44.1f - (44.1f - s->p[P_DIG_RATE]) * m;
    s->s_bits = smooth(s->s_bits, bits_t, k);
    s->s_rate = smooth(s->s_rate, rate_t, k);
    s->s_tilt = smooth(s->s_tilt, s->p[P_TONE_TILT] / 100.0f * m, k);
    s->s_spc = smooth(s->s_spc, s->p[P_SPC_AMT] / 100.0f * m, k);
    s->s_size = smooth(s->s_size, s->p[P_SPC_SIZE] / 100.0f, k);
    s->g_noise = smooth(s->g_noise, s->p[P_NOISE_ON] > 0.5f, k);
    s->g_wob = smooth(s->g_wob, s->p[P_WOB_ON] > 0.5f, k);
    s->g_drv = smooth(s->g_drv, s->p[P_DRV_ON] > 0.5f, k);
    s->g_dig = smooth(s->g_dig, s->p[P_DIG_ON] > 0.5f, k);
    s->g_tone = smooth(s->g_tone, s->p[P_TONE_ON] > 0.5f, k);
    s->g_spc = smooth(s->g_spc, s->p[P_SPC_ON] > 0.5f, k);

    const int ntype = (int)lroundf(s->p[P_NOISE_TYPE]);
    const int dtype = (int)lroundf(s->p[P_DRV_TYPE]);
    const float na = s->s_noise * s->g_noise;
    /* noise levels (amplitude, full scale = 1) */
    const float hiss_amp = (ntype == 1 ? 0.045f : 0.018f) * na * na + (ntype == 1 ? 0.008f : 0.003f) * na;
    const float crack_rate = (ntype == 0) ? (1.5f + 28.0f * na) / FS : 0;           /* clicks per sample */
    const float crack_amp = 0.35f * na;
    const float hum_amp = (ntype == 2) ? 0.05f * na : 0;
    const float k_hiss_lp = onepole_k(ntype == 1 ? 9000.0f : 4500.0f), k_hiss_hp = onepole_k(ntype == 1 ? 1500.0f : 250.0f);

    /* wobble: depth in samples; rate morphs from slow wow to fast flutter */
    const float wa = s->s_wob;
    const float wow_hz = 0.35f + 1.4f * s->s_wrate, flut_hz = 4.5f + 8.0f * s->s_wrate;
    const float wow_depth = wa * 70.0f * (1.0f - 0.6f * s->s_wrate);   /* up to ~1.6 ms */
    const float flut_depth = wa * (2.0f + 10.0f * s->s_wrate);
    const float drift_depth = wa * 40.0f;

    /* drive */
    const float da = s->s_drv;
    const float drive = 1.0f + 14.0f * da * da + 3.0f * da;
    /* level match: unity for quiet signals, then pull back as the drive compresses (roughly constant loudness) */
    const float makeup = 1.0f / ((dtype == 1 ? sat_tube(drive * 0.35f) / 0.35f : tanhf(drive * 0.35f) / 0.35f) * (1.0f + DRV_COMP * da));
    const float k_cas = onepole_k(dtype == 2 ? 13000.0f - 7000.0f * da : 20000.0f);

    /* digital */
    const float q = powf(2.0f, s->s_bits - 1.0f);
    const float hold_step = s->s_rate * 1000.0f / FS;

    /* tone: tilt around ~800 Hz. Dark: highs down up to 12 dB, lows up 2 dB. Bright: highs up 6 dB, lows down 3 dB */
    const float tt = s->s_tilt * s->g_tone;
    const float g_hi = tt < 0 ? powf(10.0f, 12.0f * tt / 20.0f) : powf(10.0f, 6.0f * tt / 20.0f);
    const float g_lo = tt < 0 ? powf(10.0f, -2.0f * tt / 20.0f) : powf(10.0f, -3.0f * tt / 20.0f);
    const float k_tilt = onepole_k(800.0f);

    /* space */
    const float fb = 0.70f + 0.26f * s->s_size, damp = 0.25f + 0.25f * (1.0f - s->s_size);
    const float spc_wet = 0.9f * s->s_spc * s->g_spc;

    for (int n = 0; n < frames; n++) {
        float dry[2] = {in_lr[2 * n] / 32768.0f, in_lr[2 * n + 1] / 32768.0f};
        float x[2] = {dry[0], dry[1]};

        /* ---- NOISE (added first, so the rest colours it too) ---- */
        if (na > 1e-5f) {
            float add_mono = 0;
            if (ntype == 0) {
                if (urand(&s->rng[2]) < crack_rate) {   /* a new click */
                    s->crack_env = crack_amp * (0.15f + 0.85f * powf(urand(&s->rng[2]), 3.0f));
                    s->crack_sign = frand(&s->rng[2]) > 0 ? 1.0f : -1.0f;
                    if (urand(&s->rng[2]) < 0.08f) s->pop_env = crack_amp * 0.6f;   /* the odd bigger, duller pop */
                }
                float click = s->crack_env * s->crack_sign * (0.6f + 0.4f * frand(&s->rng[2]));
                s->crack_env *= 0.80f;                    /* ~0.1 ms clicks */
                s->pop_lp += (s->pop_env * frand(&s->rng[2]) - s->pop_lp) * 0.15f;
                s->pop_env *= 0.995f;                     /* ~5 ms pops */
                s->rumble += (frand(&s->rng[2]) * 0.004f * na - s->rumble) * 0.002f;
                add_mono = click + s->pop_lp + s->rumble * 3.0f;
            } else if (ntype == 2) {
                s->hum_ph += 50.0f / FS; if (s->hum_ph >= 1.0f) s->hum_ph -= 1.0f;
                float ph = 2.0f * PI_F * s->hum_ph;
                add_mono = hum_amp * (0.7f * sinf(ph) + 0.25f * sinf(2 * ph) + 0.12f * fabsf(sinf(3 * ph)) - 0.076f);
            }
            for (int c = 0; c < 2; c++) {
                float w = frand(&s->rng[c]);
                s->hiss_lp[c] += (w - s->hiss_lp[c]) * k_hiss_lp;
                s->hiss_hp[c] += (s->hiss_lp[c] - s->hiss_hp[c]) * k_hiss_hp;
                float hiss = (s->hiss_lp[c] - s->hiss_hp[c]) * hiss_amp * 4.0f;
                x[c] += hiss + add_mono * (c ? 0.92f : 1.0f);
            }
        }

        /* ---- WOBBLE ---- */
        {
            s->wow_ph += wow_hz / FS; if (s->wow_ph >= 1.0f) s->wow_ph -= 1.0f;
            s->flut_ph += flut_hz / FS; if (s->flut_ph >= 1.0f) s->flut_ph -= 1.0f;
            if (--s->drift_count <= 0) { s->drift_count = 8000 + (int)(urand(&s->rng[2]) * 12000); s->drift_target = frand(&s->rng[2]); }
            s->drift += (s->drift_target - s->drift) * 0.00005f;
            float d = WOB_BASE + wow_depth * sinf(2 * PI_F * s->wow_ph) + flut_depth * sinf(2 * PI_F * s->flut_ph) + drift_depth * s->drift;
            int w = s->wpos;
            s->wbuf[0][w] = x[0]; s->wbuf[1][w] = x[1];
            s->dbuf[0][w] = dry[0]; s->dbuf[1][w] = dry[1];
            {   /* the dry signal, delayed by the same centre amount as the wet path (no comb filtering at MIX < 100%) */
                int rd = (w - (int)WOB_BASE + WOB_LEN) & (WOB_LEN - 1);
                dry[0] = s->dbuf[0][rd]; dry[1] = s->dbuf[1][rd];
            }
            float rp = (float)w - d; while (rp < 0) rp += WOB_LEN;
            int i0 = (int)rp; float fr = rp - i0;
            int im1 = (i0 - 1 + WOB_LEN) & (WOB_LEN - 1), i1 = (i0 + 1) & (WOB_LEN - 1), i2 = (i0 + 2) & (WOB_LEN - 1);
            i0 &= WOB_LEN - 1;
            for (int c = 0; c < 2; c++) {   /* cubic Hermite */
                float y0 = s->wbuf[c][im1], y1 = s->wbuf[c][i0], y2 = s->wbuf[c][i1], y3 = s->wbuf[c][i2];
                float c1 = 0.5f * (y2 - y0), c2 = y0 - 2.5f * y1 + 2.0f * y2 - 0.5f * y3, c3 = 0.5f * (y3 - y0) + 1.5f * (y1 - y2);
                float wob = ((c3 * fr + c2) * fr + c1) * fr + y1;
                /* the plain path is delayed by the same centre amount, so switching wobble never jumps in time */
                float plain = s->wbuf[c][(w - (int)WOB_BASE + WOB_LEN) & (WOB_LEN - 1)];
                x[c] = plain + (wob - plain) * s->g_wob;
            }
            s->wpos = (w + 1) & (WOB_LEN - 1);
        }

        /* ---- DRIVE ---- */
        if (s->g_drv > 1e-4f && da > 1e-4f) {
            for (int c = 0; c < 2; c++) {
                float z = x[c] * drive;
                float y = (dtype == 1 ? sat_tube(z) : sat_tape(z + 0.04f * da) - sat_tape(0.04f * da)) * makeup;
                if (dtype == 2) { s->cas_lp[c] += (y - s->cas_lp[c]) * k_cas; y = s->cas_lp[c]; }
                x[c] += (y - x[c]) * s->g_drv;
            }
        }

        /* ---- DIGITAL ---- */
        if (s->g_dig > 1e-4f) {
            s->hold_ph += hold_step;
            int take = s->hold_ph >= 1.0f;
            if (take) s->hold_ph -= floorf(s->hold_ph);
            for (int c = 0; c < 2; c++) {
                if (take || hold_step >= 0.999f) s->hold[c] = roundf(x[c] * q) / q;
                x[c] += (s->hold[c] - x[c]) * s->g_dig;
            }
        }

        /* ---- TONE (tilt) ---- */
        for (int c = 0; c < 2; c++) {
            s->tilt_lp[c] += (x[c] - s->tilt_lp[c]) * k_tilt;
            float lo = s->tilt_lp[c], hi = x[c] - lo;
            x[c] = lo * g_lo + hi * g_hi;
        }

        /* ---- SPACE ---- */
        if (spc_wet > 1e-4f) {
            float inp = (x[0] + x[1]) * 0.015f;
            for (int c = 0; c < 2; c++) {
                float acc = 0;
                for (int i = 0; i < 4; i++) {
                    comb_t *cb = &s->comb[c][i];
                    float o = cb->buf[cb->pos];
                    cb->store = o * (1 - damp) + cb->store * damp;
                    cb->buf[cb->pos] = inp + cb->store * fb;
                    if (++cb->pos >= cb->len) cb->pos = 0;
                    acc += o;
                }
                for (int i = 0; i < 2; i++) {
                    ap_t *a = &s->ap[c][i];
                    float b = a->buf[a->pos];
                    a->buf[a->pos] = acc + b * 0.5f;
                    if (++a->pos >= a->len) a->pos = 0;
                    acc = b - acc;
                }
                x[c] += acc * spc_wet * 2.0f;
            }
        }

        /* ---- MIX + OUTPUT ---- */
        for (int c = 0; c < 2; c++) {
            float y = (dry[c] + (x[c] - dry[c]) * s->s_mix) * s->s_out;
            if (!(y == y)) y = 0;                                  /* never pass a NaN on */
            /* gentle safety limiter above -1 dBFS, then a hard stop */
            if (y > 0.89f) y = 0.89f + 0.11f * tanhf((y - 0.89f) / 0.11f);
            else if (y < -0.89f) y = -0.89f - 0.11f * tanhf((-y - 0.89f) / 0.11f);
            out_lr[2 * n + c] = (int16_t)lrintf(y * 32767.0f);
        }
    }
}

static const mpc_engine_t ENGINE = { create, destroy, midi, set_param, get_param, render, process, NULL };
const mpc_engine_t *mpc_engine(void) { return &ENGINE; }
