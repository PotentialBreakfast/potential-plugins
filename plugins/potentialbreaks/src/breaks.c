/* PotentialBreaks (PotentialBreaks): a one-tap drum-break slicer for MPC OS standalone devices.
 *
 *  - Reads the break library on the MPC's drives: every sub-folder of a "DRUM BREAKS LIBRARY" folder is a library
 *    (plus "ALL"), WAV and FLAC, up to 4 folder levels deep.
 *  - RANDOM / PREV / NEXT pick a break. Long tracks (> 24 s, e.g. a whole drum-library track) get a random 1, 2 or 4 bar
 *    loop cut from them, starting on a hit; short files are used whole (they are loops already).
 *  - Tempo: the "NNbpm" in the file name when there is one, else measured (onset autocorrelation).
 *  - CHOP: an even grid of 8 or 16 slices (snapped to the nearest hit) or the 16 strongest hits.
 *  - 16 pads: MIDI notes 36-51 (MPC drum pads) and 0-15, or any 16 notes from LEARN (press LEARN, then the pad for pad 1).
 *    Note 52 (the next pad up from 51) plays the whole loop.
 *  - TEMPO: RE-PITCH (speed follows the MPC tempo, pitch moves with it) or STRETCH (slices keep their pitch; each slice is
 *    cut to its length at the MPC tempo, the classic sliced-break method).
 *  - SHUFFLE re-deals the slices over the pads; ORDER puts them back. SAVE GROOVE writes a MIDI file of the original slice
 *    order (PotentialBreaks/<name>.mid on the library's drive) to load into a sequence.
 * 44.1 kHz, interleaved int16 stereo; render() may be asked for 1..128 frames (SAMPLE_ACCURATE). */
#define _GNU_SOURCE
#include <ctype.h>
#include <dirent.h>
#include <math.h>
#include <pthread.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>
#include "engine.h"

#define DR_WAV_IMPLEMENTATION
#define DR_WAV_NO_STDIO_WIDE
#include "vendor/dr_wav.h"
#define DR_FLAC_IMPLEMENTATION
#define DR_FLAC_NO_WCHAR
#include "vendor/dr_flac.h"

#define FS 44100
#define NPADS 16
#define NVOICES 8
#define LONG_FILE_SEC 24.0
#define MAX_FILE_SEC 180.0      /* never use more than the first 3 minutes of a file (memory) */
#define HOP 441                 /* 10 ms analysis hop */

/* ---------------- parameters (same order as vst/params.json) ---------------- */
enum {
    P_RANDOM, P_PREV, P_NEXT, P_LIB_NEXT,
    P_LIB_NAME, P_BREAK_NAME, P_INFO, P_STATUS,
    P_CHOP, P_BARS, P_TEMPO_MODE, P_PITCH, P_CHOKE, P_VOLUME,
    P_SHUFFLE, P_ORDER, P_LEARN, P_PLAY_LOOP, P_SAVE_GROOVE, P_BASE_NOTE, P_RESCAN,
    NP
};
static const char *const PKEY[NP] = {
    "random", "prev", "next", "lib_next",
    "lib_name", "break_name", "info", "status",
    "chop", "bars", "tempo_mode", "pitch", "choke", "volume",
    "shuffle", "order", "learn", "play_loop", "save_groove", "base_note", "rescan",
};
enum { CHOP_GRID8, CHOP_GRID16, CHOP_HITS };
enum { TM_REPITCH, TM_STRETCH };

/* ---------------- library ---------------- */
typedef struct { char *path; const char *name; int lib; } entry_t;
typedef struct { char name[96]; int count; } lib_t;
#define MAX_LIBS 48

typedef struct {
    entry_t *e; int n, cap;
    lib_t lib[MAX_LIBS]; int nlib;      /* lib[0] = ALL */
    char root_drive[256];               /* where SAVE GROOVE writes */
} library_t;

static int has_ext(const char *n, const char *ext) {
    size_t a = strlen(n), b = strlen(ext);
    return a > b && !strcasecmp(n + a - b, ext);
}
static int audio_file(const char *n) { return n[0] != '.' && (has_ext(n, ".wav") || has_ext(n, ".flac")); }

static void lib_add(library_t *L, const char *path, int lib) {
    if (L->n == L->cap) {
        int nc = L->cap ? L->cap * 2 : 1024;
        entry_t *ne = realloc(L->e, (size_t)nc * sizeof *ne);
        if (!ne) return;
        L->e = ne; L->cap = nc;
    }
    entry_t *x = &L->e[L->n++];
    x->path = strdup(path);
    x->lib = lib;
    const char *s = strrchr(x->path, '/');
    x->name = s ? s + 1 : x->path;
}

static void scan_dir(library_t *L, const char *dir, int lib, int depth) {
    if (depth > 4) return;
    DIR *d = opendir(dir);
    if (!d) return;
    struct dirent *de;
    char p[1024];
    while ((de = readdir(d))) {
        if (de->d_name[0] == '.') continue;
        snprintf(p, sizeof p, "%s/%s", dir, de->d_name);
        struct stat st;
        if (lstat(p, &st)) continue;
        if (S_ISDIR(st.st_mode)) scan_dir(L, p, lib, depth + 1);
        else if (S_ISREG(st.st_mode) && audio_file(de->d_name) && st.st_size > 1000) lib_add(L, p, lib);
    }
    closedir(d);
}

static int cmp_entry(const void *a, const void *b) {
    const entry_t *x = a, *y = b;
    if (x->lib != y->lib) return x->lib - y->lib;
    return strcasecmp(x->path, y->path);
}
static int cmp_str(const void *a, const void *b) { return strcasecmp(*(char *const *)a, *(char *const *)b); }

/* Where the breaks are. Three ways, all at once (no duplicates):
 *   1. a folder called "DRUM BREAKS LIBRARY" at the top of any drive (/media/<drive>/DRUM BREAKS LIBRARY)
 *   2. any folder at the top of a drive with "break" in its name ("My Breaks", "Breakbeats", ...)
 *   3. folders listed in /media/<drive>/PotentialBreaks/folders.txt, one per line: a full path (/media/SDMPC/Old Loops)
 *      or a folder on that drive (Old Loops). Lines starting with # are notes.
 * Inside each one, every sub-folder is a library (with everything under it); files lying loose in it make a library
 * named after the folder itself. RESCAN looks again without reloading the plugin. */
#define MAX_ROOTS 32
#define LIST_DIR "PotentialBreaks"
#define LIST_FILE "folders.txt"

typedef struct { char path[512]; char real[512]; } root_t;

static int add_root(root_t *R, int *nr, const char *path) {
    if (*nr >= MAX_ROOTS) return 0;
    struct stat st;
    if (stat(path, &st) || !S_ISDIR(st.st_mode)) return 0;
    char rp[4096], real[512];   /* realpath() needs a PATH_MAX buffer */
    snprintf(real, sizeof real, "%s", realpath(path, rp) ? rp : path);
    size_t rl = strlen(real);
    for (int i = 0; i < *nr; i++) {   /* the same folder, or one inside / around one already listed */
        size_t ol = strlen(R[i].real);
        if (!strcmp(R[i].real, real)) return 0;
        if (rl > ol && !strncmp(real, R[i].real, ol) && real[ol] == '/') return 0;
        if (ol > rl && !strncmp(R[i].real, real, rl) && R[i].real[rl] == '/') {   /* the new one contains it: replace */
            snprintf(R[i].path, sizeof R[i].path, "%s", path);
            snprintf(R[i].real, sizeof R[i].real, "%s", real);
            return 1;
        }
    }
    snprintf(R[*nr].path, sizeof R[0].path, "%s", path);
    snprintf(R[*nr].real, sizeof R[0].real, "%s", real);
    (*nr)++;
    return 1;
}

static void read_list(root_t *R, int *nr, const char *drive) {
    char p[600];
    snprintf(p, sizeof p, "%s/" LIST_DIR "/" LIST_FILE, drive);
    FILE *f = fopen(p, "r");
    if (!f) return;
    char line[600];
    while (fgets(line, sizeof line, f)) {
        char *t = line;
        while (*t == ' ' || *t == '\t' || *t == '"') t++;
        size_t n = strlen(t);
        while (n && (t[n - 1] == '\n' || t[n - 1] == '\r' || t[n - 1] == ' ' || t[n - 1] == '\t' || t[n - 1] == '"' ||
                     t[n - 1] == '/')) t[--n] = 0;
        if (!n || *t == '#') continue;
        char q[600];
        if (*t == '/') snprintf(q, sizeof q, "%s", t);
        else snprintf(q, sizeof q, "%s/%s", drive, t);
        add_root(R, nr, q);
    }
    fclose(f);
}

static const char LIST_TEMPLATE[] =
    "# PotentialBreaks - your break folders\n"
    "#\n"
    "# Found by itself, nothing to type:\n"
    "#   - a folder called DRUM BREAKS LIBRARY at the top of any drive\n"
    "#   - any folder at the top of a drive with BREAK in its name (My Breaks, Breakbeats, ...)\n"
    "#\n"
    "# Anything else: put one folder per line below, then tap RESCAN in the plugin.\n"
    "# A full path (/media/SDMPC/Old Loops) or a folder on this drive (Old Loops).\n"
    "# Each sub-folder becomes a library you can step through with LIBRARY.\n"
    "\n";

/* the first drive that takes a file gets folders.txt with the instructions in it (once; never overwritten) */
static void write_list_template(char drives[][256], int nd, const char *prefer) {
    if (prefer && prefer[0]) {   /* the drive the breaks are on, if it takes a file */
        char d[600], p[600];
        snprintf(p, sizeof p, "%s/" LIST_DIR "/" LIST_FILE, prefer);
        int have = 0;
        for (int i = 0; i < nd && !have; i++) {
            char q[600];
            snprintf(q, sizeof q, "/media/%s/" LIST_DIR "/" LIST_FILE, drives[i]);
            have = !access(q, F_OK);
        }
        if (have) return;
        snprintf(d, sizeof d, "%s/" LIST_DIR, prefer);
        mkdir(d, 0777);
        FILE *f = fopen(p, "w");
        if (f) { fputs(LIST_TEMPLATE, f); fclose(f); return; }
    }
    for (int i = 0; i < nd; i++) {
        char p[600];
        snprintf(p, sizeof p, "/media/%s/" LIST_DIR "/" LIST_FILE, drives[i]);
        if (!access(p, F_OK)) return;
    }
    for (int i = 0; i < nd; i++) {
        char d[600], p[600];
        if (!strncmp(drives[i], "az01", 4) || !strncmp(drives[i], "acvs", 4)) continue;   /* not the internal ones */
        snprintf(d, sizeof d, "/media/%s/" LIST_DIR, drives[i]);
        mkdir(d, 0777);
        snprintf(p, sizeof p, "%s/" LIST_FILE, d);
        FILE *f = fopen(p, "w");
        if (!f) continue;
        fputs(LIST_TEMPLATE, f);
        fclose(f);
        return;
    }
}

static int is_internal(const char *d) { return !strncmp(d, "az01", 4) || !strncmp(d, "acvs", 4); }

static void library_scan(library_t *L, int *nroots_out) {
    memset(L, 0, sizeof *L);
    snprintf(L->lib[0].name, sizeof L->lib[0].name, "ALL BREAKS");
    L->nlib = 1;
    char drives[24][256]; int nd = 0;
    DIR *m = opendir("/media");
    struct dirent *de;
    if (m) {
        while ((de = readdir(m)) && nd < 24) if (de->d_name[0] != '.') snprintf(drives[nd++], 256, "%s", de->d_name);
        closedir(m);
    }
    qsort(drives, nd, sizeof drives[0], (int (*)(const void *, const void *))strcasecmp);
    static root_t R[MAX_ROOTS];
    int nr = 0;
    for (int i = 0; i < nd; i++) {   /* 1 */
        char p[600];
        snprintf(p, sizeof p, "/media/%s/DRUM BREAKS LIBRARY", drives[i]);
        add_root(R, &nr, p);
    }
    for (int i = 0; i < nd; i++) {   /* 3 */
        char p[300];
        snprintf(p, sizeof p, "/media/%s", drives[i]);
        read_list(R, &nr, p);
    }
    for (int i = 0; i < nd; i++) {   /* 2 */
        if (is_internal(drives[i])) continue;
        char p[300];
        snprintf(p, sizeof p, "/media/%s", drives[i]);
        DIR *d = opendir(p);
        if (!d) continue;
        while ((de = readdir(d))) {
            if (de->d_name[0] == '.' || !strcasestr(de->d_name, "break") || !strcasecmp(de->d_name, LIST_DIR)) continue;
            char q[600];
            snprintf(q, sizeof q, "%s/%s", p, de->d_name);
            add_root(R, &nr, q);
        }
        closedir(d);
    }
    *nroots_out = nr;
    for (int r = 0; r < nr && !L->root_drive[0]; r++)   /* SAVE GROOVE writes to the first one's drive */
        if (!strncmp(R[r].path, "/media/", 7)) {
            const char *e = strchr(R[r].path + 7, '/');
            snprintf(L->root_drive, sizeof L->root_drive, "%.*s", (int)(e ? e - R[r].path : (long)strlen(R[r].path)), R[r].path);
        }
    write_list_template(drives, nd, L->root_drive);
    for (int r = 0; r < nr; r++) {
        DIR *d = opendir(R[r].path);
        if (!d) continue;
        char *subs[MAX_LIBS]; int ns = 0, loose = -1;
        const char *rname = strrchr(R[r].path, '/');
        rname = rname ? rname + 1 : R[r].path;
        while ((de = readdir(d))) {
            if (de->d_name[0] == '.') continue;
            char q[1024];
            snprintf(q, sizeof q, "%s/%s", R[r].path, de->d_name);
            struct stat st;
            if (stat(q, &st)) continue;
            if (S_ISDIR(st.st_mode)) { if (ns < MAX_LIBS) subs[ns++] = strdup(de->d_name); }
            else if (S_ISREG(st.st_mode) && audio_file(de->d_name) && st.st_size > 1000) {
                if (loose < 0 && L->nlib < MAX_LIBS) {
                    loose = L->nlib++;
                    snprintf(L->lib[loose].name, sizeof L->lib[loose].name, "%s", rname);
                }
                if (loose >= 0) lib_add(L, q, loose);
            }
        }
        closedir(d);
        qsort(subs, ns, sizeof *subs, cmp_str);
        for (int i = 0; i < ns; i++) {
            if (L->nlib < MAX_LIBS) {
                int li = L->nlib++;
                snprintf(L->lib[li].name, sizeof L->lib[li].name, "%s", subs[i]);
                char q[1024];
                snprintf(q, sizeof q, "%s/%s", R[r].path, subs[i]);
                scan_dir(L, q, li, 1);
            }
            free(subs[i]);
        }
    }
    qsort(L->e, L->n, sizeof *L->e, cmp_entry);
    for (int i = 0; i < L->n; i++) L->lib[L->e[i].lib].count++;
    L->lib[0].count = L->n;
    int map[MAX_LIBS], k = 1;   /* drop empty libraries */
    map[0] = 0;
    for (int i = 1; i < L->nlib; i++) {
        if (L->lib[i].count) { map[i] = k; if (k != i) L->lib[k] = L->lib[i]; k++; }
        else map[i] = -1;
    }
    L->nlib = k;
    for (int i = 0; i < L->n; i++) L->e[i].lib = map[L->e[i].lib];
}

static void library_free(library_t *L) {
    for (int i = 0; i < L->n; i++) free(L->e[i].path);
    free(L->e);
    L->e = NULL; L->n = L->cap = 0;
}

/* ---------------- audio loading ---------------- */
typedef struct { float *lr; long frames; } pcm_t;   /* interleaved stereo, 44.1 kHz */

static int decode(const char *path, pcm_t *out) {
    /* read at most the first MAX_FILE_SEC seconds, so a whole album never fills the MPC's memory */
    unsigned ch = 0, rate = 0; drwav_uint64 n = 0;
    float *raw = NULL;
    if (has_ext(path, ".flac")) {
        drflac *f = drflac_open_file(path, NULL);
        if (!f) return -1;
        ch = f->channels; rate = f->sampleRate;
        drflac_uint64 want = f->totalPCMFrameCount;
        drflac_uint64 cap = (drflac_uint64)(MAX_FILE_SEC * rate);
        if (!want || want > cap) want = cap;
        if (ch && rate && want) raw = malloc((size_t)want * ch * sizeof(float));
        if (raw) n = drflac_read_pcm_frames_f32(f, want, raw);
        drflac_close(f);
    } else {
        drwav w;
        if (!drwav_init_file(&w, path, NULL)) return -1;
        ch = w.channels; rate = w.sampleRate;
        drwav_uint64 want = w.totalPCMFrameCount;
        drwav_uint64 cap = (drwav_uint64)(MAX_FILE_SEC * rate);
        if (!want || want > cap) want = cap;
        if (ch && rate && want) raw = malloc((size_t)want * ch * sizeof(float));
        if (raw) n = drwav_read_pcm_frames_f32(&w, want, raw);
        drwav_uninit(&w);
    }
    if (!raw || !ch || !rate || !n) { free(raw); return -1; }
    double ratio = (double)rate / FS;
    long frames = (long)((double)n / ratio);
    float *lr = malloc((size_t)frames * 2 * sizeof(float));
    if (!lr) { free(raw); return -1; }
    for (long i = 0; i < frames; i++) {
        double sp = i * ratio;
        long i0 = (long)sp; double fr = sp - i0;
        long i1 = i0 + 1 < (long)n ? i0 + 1 : i0;
        for (int c = 0; c < 2; c++) {
            int cc = ch > 1 ? c : 0;
            float a = raw[i0 * ch + cc], b = raw[i1 * ch + cc];
            lr[2 * i + c] = (float)(a + (b - a) * fr);
        }
    }
    free(raw);
    out->lr = lr; out->frames = frames;
    return 0;
}

/* onset strength per 10 ms hop: rise in log energy of the high-passed signal */
static float *onset_env(const float *lr, long frames, int *nh) {
    int H = (int)(frames / HOP);
    *nh = H;
    if (H < 4) return NULL;
    float *e = calloc((size_t)H, sizeof *e), *o = calloc((size_t)H, sizeof *o);
    if (!e || !o) { free(e); free(o); return NULL; }
    float prev = 0;
    for (int h = 0; h < H; h++) {
        double acc = 0;
        for (int i = 0; i < HOP; i++) {
            long k = (long)h * HOP + i;
            float m = 0.5f * (lr[2 * k] + lr[2 * k + 1]);
            float d = m - prev; prev = m;
            acc += (double)d * d + 0.1 * (double)m * m;
        }
        e[h] = logf((float)(acc / HOP) + 1e-9f);
    }
    for (int h = 1; h < H; h++) { float d = e[h] - e[h - 1]; o[h] = d > 0 ? d : 0; }
    free(e);
    return o;
}

static float bpm_from_name(const char *name) {
    for (const char *p = name; *p; p++) {
        if (!strncasecmp(p, "bpm", 3) && p > name) {
            const char *q = p;
            while (q > name && q[-1] == ' ') q--;
            const char *s = q;
            while (s > name && isdigit((unsigned char)s[-1])) s--;
            if (q - s >= 2 && q - s <= 3) {
                float b = (float)atof(s);
                if (b >= 50 && b <= 220) return b;
            }
        }
    }
    return 0;
}

/* tempo from the onset envelope's autocorrelation, 80-160 BPM (breaks) */
static float detect_bpm(const float *o, int H) {
    if (!o || H < 200) return 0;
    double best = -1; float bb = 0;
    for (float bpm = 80.0f; bpm <= 160.0f; bpm += 0.25f) {
        double lag = 6000.0 / bpm;   /* hops per beat */
        double s = 0;
        for (int m = 1; m <= 4; m++) {   /* beat, 2 beats, bar: sum the autocorrelation at multiples */
            double L = lag * m;
            int l0 = (int)L; double fr = L - l0;
            double a = 0;
            for (int h = l0 + 1; h < H; h++) a += o[h] * ((1 - fr) * o[h - l0] + fr * o[h - l0 - 1]);
            s += a / (m == 1 ? 1.0 : 1.5);
        }
        if (s > best) { best = s; bb = bpm; }
    }
    return bb;
}

/* ---------------- a loaded, chopped break ---------------- */
typedef struct {
    float *lr; long frames;          /* the loop */
    float bpm;
    int bars;
    long slice_start[NPADS], slice_len[NPADS];
    int nslices;
    char name[160];
    int entry;
} loop_t;

static void loop_free(loop_t *l) { if (l) { free(l->lr); free(l); } }

/* choose the slices of a loop */
static void chop(loop_t *L, int mode) {
    int H = 0;
    float *o = onset_env(L->lr, L->frames, &H);
    long starts[NPADS]; int n = 0;
    if (mode == CHOP_HITS && o) {
        float strength[NPADS]; long pos[NPADS]; int k = 0;
        int min_gap = (int)(6000.0f / L->bpm / 4.0f * 0.9f);   /* a 1/16 note in hops */
        if (min_gap < 4) min_gap = 4;
        for (int h = 1; h + 1 < H; h++) {
            if (o[h] < o[h - 1] || o[h] < o[h + 1] || o[h] < 0.15f) continue;
            /* keep the 16 strongest, at least a 1/16 apart */
            int clash = -1;
            for (int j = 0; j < k; j++) if (labs(pos[j] - h) < min_gap) { clash = j; break; }
            if (clash >= 0) { if (o[h] > strength[clash]) { strength[clash] = o[h]; pos[clash] = h; } continue; }
            if (k < NPADS) { strength[k] = o[h]; pos[k] = h; k++; continue; }
            int w = 0;
            for (int j = 1; j < k; j++) if (strength[j] < strength[w]) w = j;
            if (o[h] > strength[w]) { strength[w] = o[h]; pos[w] = h; }
        }
        for (int a = 0; a < k; a++) for (int b = a + 1; b < k; b++) if (pos[b] < pos[a]) { long t = pos[a]; pos[a] = pos[b]; pos[b] = t; }
        starts[n++] = 0;
        for (int j = 0; j < k && n < NPADS; j++) {
            long s = pos[j] * HOP - 88;   /* 2 ms before the hit */
            if (s > starts[n - 1] + HOP * 3) starts[n++] = s;
        }
    } else {
        int N = mode == CHOP_GRID8 ? 8 : 16;
        for (int i = 0; i < N; i++) {
            long s = (long)((double)L->frames * i / N);
            if (o && i > 0) {   /* snap to the strongest hit within +/- 30 ms */
                int h0 = (int)(s / HOP), bh = h0; float bv = 0;
                for (int h = h0 - 3; h <= h0 + 3; h++) if (h > 0 && h < H && o[h] > bv) { bv = o[h]; bh = h; }
                if (bv > 0.3f) s = (long)bh * HOP - 88;
            }
            if (s < 0) s = 0;
            starts[n++] = s;
        }
    }
    free(o);
    L->nslices = n;
    for (int i = 0; i < n; i++) {
        L->slice_start[i] = starts[i];
        long e = i + 1 < n ? starts[i + 1] : L->frames;
        L->slice_len[i] = e > starts[i] ? e - starts[i] : 1;
    }
}

/* cut the loop out of a decoded file: whole file for short loops, a random bar-aligned window for long tracks */
static loop_t *make_loop(const pcm_t *pc, const char *name, int bars, unsigned *seed, int mode) {
    loop_t *L = calloc(1, sizeof *L);
    if (!L) return NULL;
    snprintf(L->name, sizeof L->name, "%s", name);
    char *dot = strrchr(L->name, '.');
    if (dot) *dot = 0;
    int H = 0;
    float *o = onset_env(pc->lr, pc->frames, &H);
    float bpm = bpm_from_name(name);
    double dur = (double)pc->frames / FS;
    long start = 0, len = pc->frames;
    if (dur <= LONG_FILE_SEC) {
        /* a loop: trim only real silence before the first sound (-45 dB), at most 3 s */
        for (long i = 0; i < pc->frames && i < 3 * FS; i++)
            if (fabsf(pc->lr[2 * i]) > 0.0056f || fabsf(pc->lr[2 * i + 1]) > 0.0056f) { start = i - 88; break; }
        if (start < 0) start = 0;
        len = pc->frames - start;
        if (!bpm) bpm = detect_bpm(o, H);
        if (bpm > 0) {   /* whole bars: snap the tempo so the loop is exactly N bars */
            double beats = (double)len / FS * bpm / 60.0;
            int b4 = (int)lround(beats / 4.0);
            if (b4 >= 1) {
                double exact = b4 * 4.0 * 60.0 * FS / len;
                if (fabs(exact - bpm) / bpm < 0.04) bpm = (float)exact;   /* close: trust the length */
                L->bars = b4;
            }
        }
        if (bpm <= 0) bpm = 95;
        if (!L->bars) L->bars = 1;
    } else {
        if (!bpm) bpm = detect_bpm(o, H);
        if (bpm <= 0) bpm = 95;
        long want = (long)(bars * 4 * 60.0 / bpm * FS);
        if (want > pc->frames) want = pc->frames;
        /* random start on a strong hit, leaving room for the loop */
        long last = pc->frames - want;
        int tries = 0;
        start = 0;
        while (o && tries++ < 400) {
            *seed = *seed * 1664525u + 1013904223u;
            long cand = (long)(((*seed >> 8) % (unsigned)(last / HOP + 1)));
            if (cand < H && o[cand] > 1.0f) { start = cand * HOP - 88; break; }
        }
        if (start < 0) start = 0;
        if (start > last) start = last;
        len = want;
        L->bars = bars;
    }
    free(o);
    L->bpm = bpm;
    L->frames = len;
    L->lr = malloc((size_t)len * 2 * sizeof(float));
    if (!L->lr) { free(L); return NULL; }
    memcpy(L->lr, pc->lr + start * 2, (size_t)len * 2 * sizeof(float));
    /* 2 ms fade at both ends */
    for (int i = 0; i < 88 && i < len; i++) {
        float g = i / 88.0f;
        for (int c = 0; c < 2; c++) { L->lr[2 * i + c] *= g; L->lr[2 * (len - 1 - i) + c] *= g; }
    }
    chop(L, mode);
    return L;
}

/* ---------------- engine instance ---------------- */
typedef struct {
    int on; int slice; double pos; long start, end; double rate; float gain;
    long played, max_out; int fade_in, fade_out, stopping;
} voice_t;

typedef struct {
    float p[NP];
    float host_bpm;
    /* text */
    char lib_name[128], break_name[160], info[96], status[96];
    unsigned display_rev;
    pthread_mutex_t text_mx;
    /* pads */
    int perm[NPADS];           /* pad -> slice */
    int learn_armed;
    int last_note;
    voice_t v[NVOICES];
    /* loop: the audio thread owns `cur`; the worker hands a new one over through `next` */
    loop_t *cur;
    pthread_mutex_t xfer_mx;
    loop_t *next, *garbage;
    /* worker */
    pthread_t th; int th_ok;
    pthread_mutex_t mx; pthread_cond_t cv;
    int quit;
    int rq_random, rq_prev, rq_next, rq_lib, rq_rechop, rq_save, rq_restore, rq_rescan;
    char restore_name[160];
    library_t libs;
    int scanned;
    int cur_lib, cur_entry;    /* worker side */
    char file_name[256];       /* name of the decoded file (survives a RESCAN) */
    pcm_t file;                /* the decoded file the loop came from (worker side) */
    unsigned seed;
    int saved_lib; char saved_name[160];
} pb_t;

static void set_text(pb_t *s, char *dst, size_t n, const char *fmt, ...) __attribute__((format(printf, 4, 5)));
static void set_text(pb_t *s, char *dst, size_t n, const char *fmt, ...) {
    va_list ap; va_start(ap, fmt);
    pthread_mutex_lock(&s->text_mx);
    vsnprintf(dst, n, fmt, ap);
    s->display_rev++;
    pthread_mutex_unlock(&s->text_mx);
    va_end(ap);
}

static int lib_first(pb_t *s, int lib) {   /* first entry index of a library */
    if (lib <= 0) return 0;
    for (int i = 0; i < s->libs.n; i++) if (s->libs.e[i].lib == lib) return i;
    return -1;
}

static void show_lib(pb_t *s) {
    const lib_t *l = &s->libs.lib[s->cur_lib];
    set_text(s, s->lib_name, sizeof s->lib_name, "%s  (%d)", l->name, l->count);
}

static void publish(pb_t *s, loop_t *L) {
    pthread_mutex_lock(&s->xfer_mx);
    if (s->next) loop_free(s->next);
    s->next = L;
    pthread_mutex_unlock(&s->xfer_mx);
}

static void show_loop(pb_t *s, const loop_t *L) {
    set_text(s, s->break_name, sizeof s->break_name, "%s", L->name);
    set_text(s, s->info, sizeof s->info, "%.1f BPM  |  %d BAR%s  |  %d SLICES", (double)L->bpm, L->bars, L->bars == 1 ? "" : "S",
             L->nslices);
}

/* load entry i (and cut a loop) */
static void load_entry(pb_t *s, int i) {
    if (i < 0 || i >= s->libs.n) return;
    entry_t *e = &s->libs.e[i];
    set_text(s, s->status, sizeof s->status, "LOADING...");
    pcm_t pc = {0};
    if (decode(e->path, &pc)) {
        set_text(s, s->status, sizeof s->status, "CAN'T READ THIS FILE - TRY ANOTHER");
        return;
    }
    free(s->file.lr);
    s->file = pc;
    s->cur_entry = i;
    snprintf(s->file_name, sizeof s->file_name, "%s", e->name);
    loop_t *L = make_loop(&s->file, e->name, (int)s->p[P_BARS] == 0 ? 1 : (int)s->p[P_BARS] == 1 ? 2 : 4, &s->seed,
                          (int)s->p[P_CHOP]);
    if (!L) { set_text(s, s->status, sizeof s->status, "OUT OF MEMORY"); return; }
    L->entry = i;
    show_loop(s, L);
    set_text(s, s->status, sizeof s->status, "READY - PLAY THE PADS");
    pthread_mutex_lock(&s->mx);
    snprintf(s->saved_name, sizeof s->saved_name, "%s", e->path);
    pthread_mutex_unlock(&s->mx);
    publish(s, L);
}

static void rechop(pb_t *s) {
    if (!s->file.lr) return;
    loop_t *L = make_loop(&s->file, s->file_name, (int)s->p[P_BARS] == 0 ? 1 : (int)s->p[P_BARS] == 1 ? 2 : 4, &s->seed,
                          (int)s->p[P_CHOP]);
    if (!L) return;
    L->entry = s->cur_entry;
    show_loop(s, L);
    publish(s, L);
}

/* write the loop's original slice order as a MIDI file */
static void wr16(FILE *f, int v) { fputc((v >> 8) & 255, f); fputc(v & 255, f); }
static void wr32(FILE *f, long v) { wr16(f, (int)(v >> 16)); wr16(f, (int)(v & 0xffff)); }
static void wrvar(FILE *f, unsigned long v) {
    unsigned char b[5]; int n = 0;
    b[n++] = v & 127;
    while ((v >>= 7)) b[n++] = 128 | (v & 127);
    while (n--) fputc(b[n], f);
}
static void save_groove(pb_t *s, const loop_t *L, const int *perm) {
    if (!L || !s->libs.root_drive[0]) { set_text(s, s->status, sizeof s->status, "NOTHING TO SAVE"); return; }
    char dir[512], path[1024], clean[160];
    snprintf(dir, sizeof dir, "%s/PotentialBreaks", s->libs.root_drive);
    mkdir(dir, 0777);
    int k = 0;
    for (const char *c = L->name; *c && k < (int)sizeof clean - 1; c++) clean[k++] = (*c == '/' || *c == '\\') ? '-' : *c;
    clean[k] = 0;
    snprintf(path, sizeof path, "%s/%s.mid", dir, clean);
    FILE *f = fopen(path, "wb");
    if (!f) { set_text(s, s->status, sizeof s->status, "COULDN'T WRITE THE MIDI FILE"); return; }
    const int PPQ = 96;
    int inv[NPADS];   /* slice -> pad */
    for (int i = 0; i < NPADS; i++) inv[i] = i;
    for (int pad = 0; pad < NPADS; pad++) if (perm[pad] < NPADS) inv[perm[pad]] = pad;
    int base = (int)s->p[P_BASE_NOTE];
    /* track data in memory first (its length goes in the header) */
    char *buf = NULL; size_t blen = 0;
    FILE *t = open_memstream(&buf, &blen);
    long us = (long)(60000000.0 / L->bpm);
    fputc(0, t); fputc(0xFF, t); fputc(0x51, t); fputc(3, t); fputc((us >> 16) & 255, t); fputc((us >> 8) & 255, t); fputc(us & 255, t);
    long last_tick = 0;
    for (int i = 0; i < L->nslices; i++) {
        long on = (long)lround((double)L->slice_start[i] / FS * L->bpm / 60.0 * PPQ);
        long off = (long)lround((double)(L->slice_start[i] + L->slice_len[i]) / FS * L->bpm / 60.0 * PPQ) - 1;
        if (off <= on) off = on + 1;
        int note = base + inv[i];
        if (note > 127) note = 127;
        wrvar(t, (unsigned long)(on - last_tick)); fputc(0x90, t); fputc(note, t); fputc(100, t);
        wrvar(t, (unsigned long)(off - on)); fputc(0x80, t); fputc(note, t); fputc(0, t);
        last_tick = off;
    }
    long end = (long)L->bars * 4 * PPQ;
    wrvar(t, (unsigned long)(end > last_tick ? end - last_tick : 0)); fputc(0xFF, t); fputc(0x2F, t); fputc(0, t);
    fclose(t);
    fwrite("MThd", 1, 4, f); wr32(f, 6); wr16(f, 0); wr16(f, 1); wr16(f, PPQ);
    fwrite("MTrk", 1, 4, f); wr32(f, (long)blen); fwrite(buf, 1, blen, f);
    free(buf);
    fclose(f);
    set_text(s, s->status, sizeof s->status, "SAVED: PotentialBreaks/%.60s.mid", clean);
}

static void scan(pb_t *s) {
    int nroots = 0;
    library_free(&s->libs);
    library_scan(&s->libs, &nroots);
    s->scanned = 1;
    s->cur_lib = 0;
    if (!s->libs.n) {
        set_text(s, s->lib_name, sizeof s->lib_name, "NO BREAKS FOUND");
        set_text(s, s->status, sizeof s->status, "NAME A FOLDER WITH 'BREAKS' IN IT, THEN TAP RESCAN");
    } else {
        show_lib(s);
        set_text(s, s->status, sizeof s->status, "%d BREAKS IN %d FOLDER%s - TAP RANDOM BREAK", s->libs.n, nroots,
                 nroots == 1 ? "" : "S");
    }
}

static void *worker(void *arg) {
    pb_t *s = arg;
    scan(s);
    for (;;) {
        pthread_mutex_lock(&s->mx);
        while (!s->quit && !s->rq_random && !s->rq_prev && !s->rq_next && !s->rq_lib && !s->rq_rechop && !s->rq_save &&
               !s->rq_restore && !s->rq_rescan && !s->garbage)
            pthread_cond_wait(&s->cv, &s->mx);
        if (s->quit) { pthread_mutex_unlock(&s->mx); break; }
        int r = s->rq_random, pv = s->rq_prev, nx = s->rq_next, lb = s->rq_lib, rc = s->rq_rechop, sv = s->rq_save,
            rs = s->rq_restore, rsc = s->rq_rescan;
        s->rq_random = s->rq_prev = s->rq_next = s->rq_lib = s->rq_rechop = s->rq_save = s->rq_restore = s->rq_rescan = 0;
        char want[160]; snprintf(want, sizeof want, "%s", s->restore_name);
        pthread_mutex_unlock(&s->mx);
        pthread_mutex_lock(&s->xfer_mx);
        loop_t *g = s->garbage; s->garbage = NULL;
        pthread_mutex_unlock(&s->xfer_mx);
        loop_free(g);
        if (rsc) { set_text(s, s->status, sizeof s->status, "LOOKING FOR BREAKS..."); scan(s); s->cur_entry = -1; }
        if (!s->libs.n) continue;
        if (rs) {   /* a saved project: find the same file again by its path, else by its name */
            int found = -1;
            for (int i = 0; i < s->libs.n && found < 0; i++) if (!strcmp(s->libs.e[i].path, want)) found = i;
            const char *wn = strrchr(want, '/'); wn = wn ? wn + 1 : want;
            for (int i = 0; i < s->libs.n && found < 0; i++) if (!strcmp(s->libs.e[i].name, wn)) found = i;
            if (found >= 0) {
                s->cur_lib = s->saved_lib >= 0 && s->saved_lib < s->libs.nlib ? s->saved_lib : 0;
                show_lib(s);
                load_entry(s, found);
            }
            continue;
        }
        if (lb) {
            s->cur_lib = (s->cur_lib + lb) % s->libs.nlib;
            show_lib(s);
            r = 1;   /* a new library: deal a break from it straight away */
        }
        if (r || pv || nx) {
            int lib = s->cur_lib, first = lib_first(s, lib), cnt = s->libs.lib[lib].count;
            if (first < 0 || cnt <= 0) continue;
            int idx;
            if (r) { s->seed = s->seed * 1664525u + 1013904223u; idx = first + (int)((s->seed >> 8) % (unsigned)cnt); }
            else {
                int rel = (s->cur_entry >= first && s->cur_entry < first + cnt) ? s->cur_entry - first : 0;
                rel = (rel + (nx ? 1 : -1) + cnt) % cnt;
                idx = first + rel;
            }
            load_entry(s, idx);
        } else if (rc) rechop(s);
        if (sv) {
            pthread_mutex_lock(&s->xfer_mx);
            const loop_t *L = s->next ? s->next : s->cur;   /* the newest loop */
            int perm[NPADS]; memcpy(perm, s->perm, sizeof perm);
            if (L) save_groove(s, L, perm);
            pthread_mutex_unlock(&s->xfer_mx);
        }
    }
    return NULL;
}

static void wake(pb_t *s, int *flag, int val) {
    pthread_mutex_lock(&s->mx);
    *flag = val;
    pthread_cond_signal(&s->cv);
    pthread_mutex_unlock(&s->mx);
}

static const float PDEF[NP] = {0, 0, 0, 0, 0, 0, 0, 0, CHOP_GRID16, 1, TM_REPITCH, 0, 1, 0, 0, 0, 0, 0, 0, 36, 0};

static void *create(const char *data_dir) {
    (void)data_dir;
    pb_t *s = calloc(1, sizeof *s);
    if (!s) return NULL;
    for (int i = 0; i < NP; i++) s->p[i] = PDEF[i];
    for (int i = 0; i < NPADS; i++) s->perm[i] = i;
    s->host_bpm = 0;
    s->last_note = -1;
    s->saved_lib = -1;
    s->seed = (unsigned)time(NULL) ^ (unsigned)(uintptr_t)s;
    pthread_mutex_init(&s->text_mx, NULL);
    pthread_mutex_init(&s->xfer_mx, NULL);
    pthread_mutex_init(&s->mx, NULL);
    pthread_cond_init(&s->cv, NULL);
    snprintf(s->lib_name, sizeof s->lib_name, "SCANNING...");
    snprintf(s->break_name, sizeof s->break_name, "-");
    snprintf(s->info, sizeof s->info, "-");
    snprintf(s->status, sizeof s->status, "LOOKING FOR YOUR BREAKS...");
    pthread_attr_t at;
    pthread_attr_init(&at);
    pthread_attr_setstacksize(&at, 512 * 1024);
    s->th_ok = pthread_create(&s->th, &at, worker, s) == 0;
    pthread_attr_destroy(&at);
    return s;
}

static void destroy(void *inst) {
    pb_t *s = inst;
    if (s->th_ok) {
        pthread_mutex_lock(&s->mx); s->quit = 1; pthread_cond_signal(&s->cv); pthread_mutex_unlock(&s->mx);
        pthread_join(s->th, NULL);
    }
    loop_free(s->cur); loop_free(s->next); loop_free(s->garbage);
    free(s->file.lr);
    library_free(&s->libs);
    pthread_mutex_destroy(&s->text_mx); pthread_mutex_destroy(&s->xfer_mx); pthread_mutex_destroy(&s->mx);
    pthread_cond_destroy(&s->cv);
    free(s);
}

/* ---------------- playing ---------------- */
static void start_voice(pb_t *s, int slice, int vel, int whole) {
    loop_t *L = s->cur;
    if (!L) return;
    if (!whole && (slice < 0 || slice >= L->nslices)) return;
    if (s->p[P_CHOKE] > 0.5f || whole)
        for (int i = 0; i < NVOICES; i++) if (s->v[i].on && !s->v[i].stopping) { s->v[i].stopping = 1; s->v[i].fade_out = 132; }
    int k = -1;
    for (int i = 0; i < NVOICES; i++) if (!s->v[i].on) { k = i; break; }
    if (k < 0) {   /* steal the oldest */
        long most = -1;
        for (int i = 0; i < NVOICES; i++) if (s->v[i].played > most) { most = s->v[i].played; k = i; }
    }
    voice_t *v = &s->v[k];
    memset(v, 0, sizeof *v);
    v->on = 1;
    v->slice = whole ? -1 : slice;
    v->start = whole ? 0 : L->slice_start[slice];
    v->end = whole ? L->frames : L->slice_start[slice] + L->slice_len[slice];
    v->pos = (double)v->start;
    float tune = powf(2.0f, s->p[P_PITCH] / 12.0f);
    float hb = s->host_bpm > 0 ? s->host_bpm : L->bpm;
    int stretch = (int)s->p[P_TEMPO_MODE] == TM_STRETCH;
    v->rate = stretch ? tune : (hb / L->bpm) * tune;
    /* stretch: the slice may sound only as long as it lasts at the MPC tempo */
    v->max_out = stretch ? (long)((double)(v->end - v->start) * L->bpm / hb) : -1;
    float g = vel / 127.0f;
    v->gain = g * g * powf(10.0f, s->p[P_VOLUME] / 20.0f);
    v->fade_in = 22;
}

static int note_to_pad(pb_t *s, int note) {
    int base = (int)s->p[P_BASE_NOTE];
    if (note >= base && note < base + NPADS) return note - base;
    if (note >= 36 && note < 52) return note - 36;
    if (note >= 0 && note < 16) return note;
    return -1;
}

static void midi(void *inst, const uint8_t *msg, int len) {
    pb_t *s = inst;
    if (len < 3) return;
    int st = msg[0] & 0xF0, note = msg[1], vel = msg[2];
    if (st != 0x90 || vel == 0) return;   /* one-shots: note-offs are ignored */
    s->last_note = note;
    if (s->learn_armed) {
        s->learn_armed = 0;
        s->p[P_BASE_NOTE] = (float)note;
        set_text(s, s->status, sizeof s->status, "LEARNED: PAD 1 = NOTE %d", note);
        return;
    }
    int base = (int)s->p[P_BASE_NOTE];
    if (note == base + NPADS || note == 52) { start_voice(s, 0, vel, 1); return; }   /* the next pad up: whole loop */
    int pad = note_to_pad(s, note);
    if (pad < 0) return;
    int sl = s->perm[pad];
    int ns = s->cur ? s->cur->nslices : NPADS;
    if (ns > 0 && sl >= ns) sl %= ns;   /* fewer hits than pads: the rest repeat from the start */
    start_voice(s, sl, vel, 0);
}

static void render(void *inst, int16_t *out_lr, int frames) {
    pb_t *s = inst;
    if (pthread_mutex_trylock(&s->xfer_mx) == 0) {   /* take a new loop when one is ready */
        if (s->next) {
            if (!s->garbage) {
                s->garbage = s->cur;
                s->cur = s->next;
                s->next = NULL;
                for (int i = 0; i < NVOICES; i++) s->v[i].on = 0;
                pthread_mutex_unlock(&s->xfer_mx);
                pthread_mutex_lock(&s->mx); pthread_cond_signal(&s->cv); pthread_mutex_unlock(&s->mx);
            } else pthread_mutex_unlock(&s->xfer_mx);
        } else pthread_mutex_unlock(&s->xfer_mx);
    }
    loop_t *L = s->cur;
    for (int n = 0; n < frames; n++) {
        float acc[2] = {0, 0};
        if (L) for (int i = 0; i < NVOICES; i++) {
            voice_t *v = &s->v[i];
            if (!v->on) continue;
            long i0 = (long)v->pos;
            if (i0 + 1 >= v->end || (v->max_out >= 0 && v->played >= v->max_out && !v->stopping)) {
                if (i0 + 1 >= v->end) { v->on = 0; continue; }
                v->stopping = 1; v->fade_out = 132;
            }
            double fr = v->pos - i0;
            float g = v->gain;
            if (v->fade_in > 0) { g *= 1.0f - v->fade_in / 22.0f; v->fade_in--; }
            /* a short fade at the slice end too */
            long left = (long)((v->end - v->pos) / v->rate);
            if (left < 88) g *= left / 88.0f;
            if (v->stopping) {
                g *= v->fade_out / 132.0f;
                if (--v->fade_out <= 0) { v->on = 0; continue; }
            }
            for (int c = 0; c < 2; c++) {
                float a = L->lr[2 * i0 + c], b = L->lr[2 * (i0 + 1) + c];
                acc[c] += (a + (b - a) * (float)fr) * g;
            }
            v->pos += v->rate;
            v->played++;
        }
        for (int c = 0; c < 2; c++) {
            float y = acc[c];
            if (y > 0.98f) y = 0.98f + 0.02f * tanhf((y - 0.98f) / 0.02f);
            else if (y < -0.98f) y = -0.98f - 0.02f * tanhf((-y - 0.98f) / 0.02f);
            out_lr[2 * n + c] = (int16_t)lrintf(y * 32767.0f);
        }
    }
}

/* ---------------- parameters ---------------- */
static int find(const char *key) {
    for (int i = 0; i < NP; i++) if (!strcmp(key, PKEY[i])) return i;
    return -1;
}

static void shuffle(pb_t *s) {
    int n = s->cur ? s->cur->nslices : NPADS;
    if (n < 2) return;
    for (int i = n - 1; i > 0; i--) {
        s->seed = s->seed * 1664525u + 1013904223u;
        int j = (int)((s->seed >> 8) % (unsigned)(i + 1));
        int t = s->perm[i]; s->perm[i] = s->perm[j]; s->perm[j] = t;
    }
    set_text(s, s->status, sizeof s->status, "SHUFFLED - TAP ORDER TO PUT THEM BACK");
}

static void set_param(void *inst, const char *key, const char *val) {
    pb_t *s = inst;
    if (!strcmp(key, "lfo_bpm")) { s->host_bpm = (float)atof(val); return; }
    if (!strcmp(key, "state")) {   /* "chop=1;bars=1;...;lib=3;file=/media/...;perm=0,1,2,..." */
        char tmp[2048]; snprintf(tmp, sizeof tmp, "%s", val);
        char *save = NULL;
        for (char *tok = strtok_r(tmp, ";", &save); tok; tok = strtok_r(NULL, ";", &save)) {
            char *eq = strchr(tok, '=');
            if (!eq) continue;
            *eq = 0;
            const char *v = eq + 1;
            if (!strcmp(tok, "file")) {
                pthread_mutex_lock(&s->mx);
                snprintf(s->restore_name, sizeof s->restore_name, "%s", v);
                s->rq_restore = 1;
                pthread_cond_signal(&s->cv);
                pthread_mutex_unlock(&s->mx);
            } else if (!strcmp(tok, "lib")) s->saved_lib = atoi(v);
            else if (!strcmp(tok, "perm")) {
                int k = 0; char pv[256]; snprintf(pv, sizeof pv, "%s", v);
                char *sv2 = NULL;
                for (char *t = strtok_r(pv, ",", &sv2); t && k < NPADS; t = strtok_r(NULL, ",", &sv2)) s->perm[k++] = atoi(t) & 15;
            } else {
                int i = find(tok);
                if (i >= 0) s->p[i] = (float)atof(v);
            }
        }
        return;
    }
    int i = find(key);
    if (i < 0) return;
    float v = (float)atof(val);
    int press = v > 0.5f;
    switch (i) {
    case P_RANDOM: if (press) wake(s, &s->rq_random, 1); return;
    case P_PREV: if (press) wake(s, &s->rq_prev, 1); return;
    case P_NEXT: if (press) wake(s, &s->rq_next, 1); return;
    case P_LIB_NEXT: if (press) wake(s, &s->rq_lib, 1); return;
    case P_SHUFFLE: if (press) shuffle(s); return;
    case P_ORDER:
        if (press) { for (int k = 0; k < NPADS; k++) s->perm[k] = k; set_text(s, s->status, sizeof s->status, "SLICES IN ORDER"); }
        return;
    case P_LEARN:
        if (press) { s->learn_armed = 1; set_text(s, s->status, sizeof s->status, "LEARN: HIT THE PAD YOU WANT AS PAD 1"); }
        return;
    case P_PLAY_LOOP: if (press) start_voice(s, 0, 110, 1); return;
    case P_SAVE_GROOVE: if (press) wake(s, &s->rq_save, 1); return;
    case P_RESCAN: if (press) wake(s, &s->rq_rescan, 1); return;
    case P_LIB_NAME: case P_BREAK_NAME: case P_INFO: case P_STATUS: return;
    case P_CHOP: case P_BARS: {
        int changed = (int)lroundf(s->p[i]) != (int)lroundf(v);
        s->p[i] = v;
        if (changed) wake(s, &s->rq_rechop, 1);
        return;
    }
    default: s->p[i] = v; return;
    }
}

static int get_param(void *inst, const char *key, char *buf, int buf_len) {
    pb_t *s = inst;
    if (!strcmp(key, "display_rev")) {
        pthread_mutex_lock(&s->text_mx);
        int n = snprintf(buf, buf_len, "%u", s->display_rev);
        pthread_mutex_unlock(&s->text_mx);
        return n;
    }
    if (!strcmp(key, "state")) {
        int n = 0;
        static const int keep[] = {P_CHOP, P_BARS, P_TEMPO_MODE, P_PITCH, P_CHOKE, P_VOLUME, P_BASE_NOTE};
        for (size_t k = 0; k < sizeof keep / sizeof *keep; k++)
            n += snprintf(buf + n, buf_len - n, "%s=%g;", PKEY[keep[k]], (double)s->p[keep[k]]);
        pthread_mutex_lock(&s->mx);
        n += snprintf(buf + n, buf_len - n, "lib=%d;file=%s;perm=", s->cur_lib, s->saved_name);
        pthread_mutex_unlock(&s->mx);
        for (int k = 0; k < NPADS && n < buf_len; k++) n += snprintf(buf + n, buf_len - n, "%d%s", s->perm[k], k < NPADS - 1 ? "," : "");
        return n;
    }
    int i = find(key);
    if (i < 0) return 0;
    const char *txt = NULL;
    pthread_mutex_lock(&s->text_mx);
    char t[200];
    switch (i) {
    case P_LIB_NAME: txt = s->lib_name; break;
    case P_BREAK_NAME: txt = s->break_name; break;
    case P_INFO: txt = s->info; break;
    case P_STATUS:
        if (s->last_note >= 0 && !strncmp(s->status, "READY", 5)) {
            snprintf(t, sizeof t, "READY  |  LAST NOTE %d", s->last_note);
            txt = t;
        } else txt = s->status;
        break;
    default: break;
    }
    int n;
    if (txt) n = snprintf(buf, buf_len, "%s", txt);
    else if (i == P_RANDOM || i == P_PREV || i == P_NEXT || i == P_LIB_NEXT || i == P_SHUFFLE || i == P_ORDER || i == P_LEARN ||
             i == P_PLAY_LOOP || i == P_SAVE_GROOVE || i == P_RESCAN)
        n = snprintf(buf, buf_len, "0");
    else n = snprintf(buf, buf_len, "%g", (double)s->p[i]);
    pthread_mutex_unlock(&s->text_mx);
    return n;
}

static const mpc_engine_t ENGINE = { create, destroy, midi, set_param, get_param, render, NULL, NULL };
const mpc_engine_t *mpc_engine(void) { return &ENGINE; }
