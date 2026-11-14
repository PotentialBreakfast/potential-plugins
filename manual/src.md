# PotentialSuite Manual

*Eleven free plugins for Akai MPC standalone hardware, by PotentialBreaks.*

{{EDITION}}

## About this manual

The **user guide** comes first: what each plugin does, how to install and load them, and how they work together, with the preset chains and the order to load them in. The **developer reference** is at the back: how the plugins are built, their parameters and file formats, and how to build, test and release one.

Every plugin also carries its own short manual on the MPC's screen: open the plugin and tap **MANUAL**.

The newest copy of this manual is at [github.com/PotentialBreakfast/potential-plugins](https://github.com/PotentialBreakfast/potential-plugins).

{{TOC}}

# Part 1: User guide

## 1. The suite at a glance

PotentialSuite is eleven plugins that run inside the MPC like its own instruments and effects. Each has its own touchscreen pages, works with the Q-Links and is saved with your project. No computer is needed while you play.

| Plugin | Loads as | What it is |
|---|---|---|
| **PotentialBreaks** | Instrument | Drum-break slicer: a random break from your own library on 16 pads, chopped and matched to your tempo |
| **PotentialWax** | Instrument | Melodic sampler: songs and loops on 16 pads, with tempo and key matched |
| **PotentialCrates** | Instrument | Record digger: finds records on Discogs and streams one into your track (needs a network connection) |
| **PotentialKits** | Instrument | Drum-kit builder: 16 sounds from your own sample folders, with kit styles, ROLL, and pitch, decay and choke per pad |
| **PotentialNoise** | Instrument | Mixing tool: calibrated pink noise, test tones and a speaker check |
| **PotentialPatina** | Insert&nbsp;effect | Retro colour: vinyl and tape noise, wobble, drive, bit-crush, tone and room |
| **PotentialPunch** | Insert&nbsp;effect | Transient shaper: more or less attack and sustain |
| **PotentialHeat** | Insert&nbsp;effect | Soft clipper and bus saturator: warmth and loudness without harsh peaks |
| **PotentialGrit** | Insert&nbsp;effect | Seven lo-fi effects on pads: dust, crush, filter, kill EQ, echo, chop and ring |
| **PotentialBleed** | Insert&nbsp;effect | Ghost bleed: quiet synthetic residue behind each hit |
| **PotentialWarp** | Insert&nbsp;effect | Tempo-locked mangler: half time, tape stop, stutter, reverse and gates |

**Instruments** are loaded as a plugin program on a track, and you play them from the pads. **Insert effects** go on the inserts of a track, program, pad or bus; they are not in the instrument list.

### How they fit together

A beat usually moves through the suite in this order:

1. **Dig.** PotentialCrates finds a record. PotentialBreaks, PotentialWax and PotentialKits dig through your own folders.
2. **Chop and build.** Breaks slices drums across the pads. Wax puts 16 cues on a song and matches its tempo and key. Kits builds a drum kit from your one-shots.
3. **Colour.** Punch, Grit, Bleed and Patina shape and age the sound. Four ready-made chains set them up together (chapter 6).
4. **Glue and bend.** Heat warms a bus and catches peaks. Warp adds half time, tape stops and stutters in time.
5. **Balance.** PotentialNoise gives you a quick, neutral rough mix.

### Versions covered

{{VERSIONS}}

## 2. Getting started

### What you need

- A standalone MPC on **MPC OS 3.x**. On **MPC OS 3.10.1 or later** no modification is needed. On older MPC OS 3.x you need a first-generation MPC with the community plugin set-up from [Open MPC](https://open-mpc.github.io/).
- A drive the MPC can see (USB stick, SD card, SSD or the internal drive).
- MPC OS 2.x is not supported.

Chapter 9 lists the MPCs each plugin has been tested on.

### Install: three ways

Install each plugin one way only, or it shows up twice.

**A. By hand, on MPC OS 3.10.1 or later**

1. On the plugin's GitHub page, open **Releases** and download the file ending `-akai-vst2.zip`. Unzip it on your computer.
2. On the MPC, turn on **Preferences → Enhancements (Beta) → Enable Enhancements** and **External Plugin Support**.
3. Copy the plugin's folder (for example `PotentialBreaks - VST - PotentialPunch`) into a folder called **VST2 Plugins** at the top of a drive.
4. Restart the MPC. The plugin is under **External Plugins**.

**B. With MPC TOOLS (one tap, for MPCs with SSH access)**

[MPC TOOLS](https://github.com/PotentialBreakfast/mpc-tools) is a free second-screen app for Windows and Mac. Its **Plugins** tab lists every PotentialSuite plugin with the version on your MPC and the newest one. Tap **UPDATE ALL**, then **RESTART MPC APP**. Old copies go to a backup folder on the drive, and from MPC TOOLS 1.5.6 an update keeps the look you chose and your own files.

**C. With the Open MPC installer (older MPC OS 3.x, first-generation MPCs)**

Download the file ending `-mpc-armv7.zip` and leave it zipped. Open the free **MPC Plugin Installer** from [Open MPC](https://open-mpc.github.io/), connect, drop the zip in and press Install. Save your project first, because installing restarts the MPC app. Second-generation MPCs use the file ending `-mpc-aarch64.zip` with the Open MPC Plugin Manager.

### Loading a plugin

- **Breaks, Wax, Crates, Kits and Noise** are instruments. Make a plugin track or plugin program and pick the plugin from the instrument list.
- **Patina, Punch, Heat, Grit, Bleed and Warp** are insert effects. Open the inserts of a track, program, pad, bus or the master, tap an empty slot and pick the plugin from the effect list.

### Network safety

MPC TOOLS and the Open MPC installer reach the MPC over SSH. If your MPC has SSH switched on:

- Keep it on your home network. Don't join public or shared Wi-Fi with SSH on.
- Use an SSH key instead of a password where you can. The MPC TOOLS README explains how under "SSH key setup".

The plugins themselves never open a network connection, except PotentialCrates, which searches Discogs and streams audio.

## 3. What every plugin shares

### Pages

Each plugin has tabs along the top of its screen. The first is the main page. Every plugin also has:

- **MANUAL**: the plugin's own manual, a few pages of text with page buttons along the top.
- **SETTINGS** (SETUP in PotentialWax): the LOOK selector.
- **USER** (Patina, Punch, Heat, Grit and Bleed): eight preset slots of your own (chapter 7). PotentialKits has eight USER KITS on its SAVE page.

![The MANUAL page in PotentialPunch](manual/images/manual-page.png)

### Three looks

Every plugin comes in **Print Plate** (the default), **Night Rack** and **Colour Block**. Open **SETTINGS** and tap one of the LOOK keys. The new look shows the next time the plugin loads: take it off the track and add it again, or load your project again. The choice is kept on your MPC for every project. The sound and the controls are the same in all three.

### ROLL

**ROLL** is a dice button: one tap for a random setting that still sounds good. **MILD**, **MEDIUM** or **WILD** next to it sets how far it goes. Keep rolling until you like something, then fine-tune. In Breaks and Wax, ROLL also picks a random break or song. In Crates it picks the search filters. In Kits it rolls new samples, and at MEDIUM and WILD pitch and decay too.

### Presets

Presets are in two places, depending on the plugin:

- **On the page**, as one-tap buttons: Punch and Heat (PRESETS), Patina (ERA), Warp (PATTERNS).
- **In the MPC's PRESET menu** at the top of the plugin screen: Patina, Grit, Bleed, Punch, Heat and Warp.

A preset is a starting point. Tweak from there.

### Q-Links

The main controls of each page are on the Q-Links. The plugin chapters list them.

### Saved with your project

Every setting is saved with your MPC project and comes back when you load it. Breaks and Wax also remember the break or song, the chops and the cues. The look and the USER slots are not part of a project: they belong to the MPC.

## 4. The instruments

### 4.1 PotentialBreaks

![PotentialBreaks](docs/images/potentialbreaks.png)

**Dig, chop, play.** One tap for a random drum break from your own library, sliced across 16 pads and matched to your project's tempo.

**Quick start**

1. Put your breaks (WAV, FLAC or MP3) in a folder called `DRUM BREAKS LIBRARY` at the top of a drive.
2. Load PotentialBreaks as a plugin instrument.
3. Tap **RANDOM BREAK**, or **ROLL** for a random break played a random way.
4. Play the pads.

**Controls**

| Control | What it does |
|---|---|
| **RANDOM BREAK** | Picks a break from your library |
| **PREV / NEXT** | Step through the breaks |
| **LIBRARY** | Switches folders. Every sub-folder is a library |
| **BROWSE** page | Lists 10 breaks at a time; tap the one you want. **RESCAN** after adding folders |
| **ROLL** | A random break and a random way to play it: chop, loop length, pad order and pitch |
| **CHOP** | 8 or 16 even slices on the beat (moved onto the nearest hit), or the 16 strongest hits |
| **TEMPO** | **Re-pitch** speeds the break up or down to your tempo, like an old sampler. **Stretch** keeps each slice at its own pitch and cuts it to length |
| **SHUFFLE / ORDER** | Re-deals the slices across the pads, or puts them back |
| **PLAY LOOP** | Plays the whole loop (so does the pad after pad 16) |
| **CHOKE, PITCH, VOLUME** | Pads cut each other off; ±12 semitones; level |
| **SAVE GROOVE** | Writes a MIDI file of the break's original slice order |
| **SAVE KIT** | Writes a real MPC drum program: each pad as a WAV, as you hear it, on pads A01 to A16, with the groove as a MIDI file. MPC OS 3.10.1 or later |
| **LEARN** | Sets which pad is pad 1 |

**Q-Links:** PITCH, VOLUME, CHOP, BARS, TEMPO and CHOKE.

**Where it finds your breaks**

1. A folder called `DRUM BREAKS LIBRARY` at the top of any drive.
2. Any folder at the top of a drive with "break" in its name.
3. Folders you list in `PotentialBreaks/folders.txt` on the breaks drive, one per line. The plugin creates this file the first time it runs.

**Good to know**

- Tempo comes from the file name when it has one (`Funky Drummer 101bpm.wav`, or `.mp3`). Otherwise it is measured, between 80 and 160 BPM, so a 70 BPM break may read as 140.
- Long tracks get a random 1, 2 or 4 bar loop cut from them, starting on a hit. Only the first 3 minutes of a file are used.
- Stretch mode does not time-stretch: at slower tempos you hear small gaps between hits, the classic chopped sound.
- 8 voices, 44.1 kHz playback.
- Kits are saved to `PotentialBreaks/Kits` on the breaks drive, grooves to the `PotentialBreaks` folder. To use your own programs folder, put its full path on the first line of `PotentialBreaks/kit-folder.txt`.

### 4.2 PotentialWax

![PotentialWax](docs/images/potentialwax.png)

**Dig, flip, play.** A melodic sampler: pick a song from your own library, or let it dig one out, and it finds 16 cue points, works out the tempo and key, and matches them to your project.

**Quick start**

1. Put songs or loops (MP3, WAV or FLAC) in a folder called `MELODIC LIBRARY` at the top of a drive. Wax needs a folder of songs to work.
2. Load PotentialWax as a plugin instrument. Tap **RESCAN** on the BROWSE page after adding songs.
3. Tap **RANDOM TRACK**, or **ROLL** for a random song played a random way.
4. Play the pads. Each pad starts at a cue.

**WAX page**

| Control | What it does |
|---|---|
| **RANDOM TRACK, PREV / NEXT, LIBRARY** | Dig a song, step through them, switch folders |
| **CUES ON** | Where the 16 cues go: **Phrases** (4-bar phrase starts), **Bars** or **Hits** |
| **FIND CUES** | Rolls a new set of cues |
| **NUDGE < >** | Moves the last pad's cue |
| **BEAT CHOP** | Puts the 16 pads one beat apart: 4 bars, from the bar of the last pad you played |
| **MODE** | **Pads**: each pad plays from its cue. **Keys**: the last pad, spread over the keyboard from the ROOT note |
| **PAD PLAYS** | Hold, To Next Cue, 1 Bar or 2 Bars |
| **TEMPO** | Original, or Match MPC |
| **KEY** | Original, or any key from C to B (it moves the shortest way). **PITCH** adds ±12 semitones |
| **KEY LOCK** | Keeps one key for everything you dig |
| **PITCHING** | **Keep Tempo** (time-stretch) or **Vinyl** (speed and pitch move together) |
| **÷2 / ×2** | Halve or double the tempo it found |
| **PLAY SONG / STOP** | Plays the whole song |
| **SAVE KIT** | Writes an MPC drum program of the 16 pads as you hear them, with a MIDI file. MPC OS 3.10.1 or later |
| **SAVE PADS** | Writes the 16 chops as WAV files |

**WAVE page**

The song's waveform with all 16 cues. Playing a pad selects it. While a pad plays, the playhead (a bright line with a notch at the top and bottom) moves along the waveform and the part already played lights up in Wax's colour. For the selected pad:

- **FLIP:** **REVERSE**, **HALF SPEED** (an octave down) and **FILTER** (a low-pass). Each pad keeps its own, and SAVE KIT saves the pads flipped.
- **VIEW:** the whole song, or 16, 4 or 1 bar around the cue.
- **Move the cue** by a bar, a beat, a 1/16 note or a tiny step (5 ms). **SNAP TO HIT** moves it onto the nearest real hit; **RESET CUE** puts it back.

**Q-Links:** PITCH, VOLUME, TEMPO, KEY, CUES ON, PAD PLAYS, MODE and PITCHING (WAX page); the view and the flips (WAVE page); RELEASE, CHOKE and the pad 1 and ROOT notes (SETUP page).

**Where it finds your songs**

1. A folder called `MELODIC LIBRARY` at the top of any drive.
2. Any folder at the top of a drive with "melodic", "wax", "flip" or "songs" in its name.
3. Folders you list in `PotentialWax/folders.txt` on that drive, one per line.

**Good to know**

- The MPC does not tell plugins the project's key, so KEY is where you set it.
- A file that is a whole number of bars (1, 2, 4, 8, 16 or 32) is recognised as a cut loop and gets its exact tempo from its length.
- Tempo is found between 75 and 150 BPM, or read from a name like `92bpm`. Tempo, key and cues are a best guess.
- Only the first 6 minutes of a song are used. Files shorter than about 2 seconds are skipped.
- 8 voices, 44.1 kHz playback. CHOKE, RELEASE and LEARN are on the SETUP page, with the LOOK selector.
- Kits are saved to `PotentialWax/Kits` on the song's drive, pads to the `PotentialWax` folder.

### 4.3 PotentialCrates

![PotentialCrates](docs/images/potentialcrates.png)

**Dig for records, one tap at a time.** PotentialCrates finds records on Discogs and streams one into your MPC track, ready to sample. It needs a network connection.

**Quick start**

1. Connect the MPC to your network. Load PotentialCrates as a plugin instrument.
2. Open the **CRATES** page and tap a crate. It searches and plays the first result.
3. Or open **DIG** and tap **SEARCH**, then tap a result.

**The eight crates**

| Crate | Digs in |
|---|---|
| **DRUM BREAKS** | Funk, Soul, Jazz-Funk, Boogaloo and Afrobeat, 60s and 70s |
| **LIBRARY** | Library records across Stage & Screen, Jazz and Electronic, 60s to 80s |
| **SOUNDTRACKS** | Soundtrack and Score, 60s to 80s |
| **JAZZ-FUNK** | Jazz-Funk, Soul-Jazz and Fusion |
| **PSYCH** | Psychedelic Rock, Krautrock, psychedelic Soul |
| **BOSSA & SAMBA** | Bossanova, Samba, MPB, Bossa Nova jazz |
| **DISCO & BOOGIE** | Disco and Boogie, 70s and 80s |
| **DUB & REGGAE** | Dub, Roots Reggae, Rocksteady |

**Controls**

| Control | What it does |
|---|---|
| **SEARCH** (DIG and FILTERS pages) | Digs Discogs with your filters. Leave them on ANY for anything at all |
| **FILTERS** page | Genre, style, decade, region and country, with arrows. **CLEAR FILTERS** sets them back to ANY |
| **ROLL** (FILTERS and CRATES pages) | Picks the filters for you, searches and plays the result. MILD stays near the genre you are in; WILD picks anything |
| **SOURCE: CHANNEL** | Digs random uploads from YouTube channels you list in `channels.txt` in the plugin's folder (see below) |
| **RESULTS** | 8 a page. Tap one to play it. **PAGE UP / PAGE DOWN** |
| **PLAY, STOP, BACK 15, FWD 15, NEXT** | The transport |
| **AUTO NEXT** | Moves to the next result when a track ends or won't play (on by default) |
| **TRANSPORT SYNC** | A track loads and waits, silent, until you press Play on the MPC, and the MPC's Stop pauses it. For resampling in time |

**Good to know**

- **`channels.txt`** is made the first time PotentialCrates starts: a copy of your old Crate Digger channel list if one is on the same drive, else a short how-to with examples. Write one channel a line (a channel link, an `@handle`, a `UC...` ID or a playlist link, then `|` and a name if you like). Updates never change a list with your own channels in it.
- If a dig finds nothing, it digs again with new picks, up to 3 times.
- The status line says why a track won't play: private video, removed, region blocked, rate limited or no network.
- The download is large (about 70 MB for a first-generation MPC, 90 MB for a second-generation one) because it carries its own streaming engine.
- Check that you have the right to use what you sample before you release it.
- PotentialCrates is a fork of Crate Digger by sd88me, built on Charles Vestal's schwung-webstream.

### 4.4 PotentialKits

![PotentialKits](docs/images/potentialkits.png)

**A drum kit from your own samples.** PotentialKits reaches into your sample folders and pulls out 16 sounds for the pads: a kick, a snare, hats, percussion and a few surprises. Lock what you like, roll the rest, shape each pad, then save it as a real MPC drum program. It is a fork of sd88me's Lucky Dip.

**Quick start**

1. Load PotentialKits as a plugin instrument. The first time, it scans the **Expansions** and **Samples** folders on every drive (pick another folder on the SETUP page).
2. On the **KIT** page tap **NEW KIT** or **ROLL**, or tap a **KIT STYLE** on the STYLES page.
3. Play the pads, or tap a tile on the KIT page to hear it.
4. **LOCK** the pads you like and roll again. When the kit is right, open the **SAVE** page and tap **EXPORT KIT**.

**Pages**

| Page | What's on it |
|---|---|
| **KIT** | **NEW KIT**, **ROLL**, the 16 pad tiles, laid out like the MPC's pads (A01 bottom left; `*` = locked), and the selected pad: its sample, **TYPE**, **LOCK**, **REROLL**, **GAIN**, **PITCH** (±12 semitones), **DECAY** (100 % = the whole sample) and **CHOKE** (OFF or group 1-4) |
| **STYLES** | **KIT STYLES** (CLASSIC, BOOM BAP, LO-FI, TRAP, HOUSE, DUSTY JAZZ, PERCUSSION, ELECTRO, WEIRD), **ROLL KIT** with MILD / MEDIUM / WILD, **UNLOCK ALL**, **CLEAR ALL** |
| **SAVE** | **USER KITS** (tap **SAVE**, then a slot), **EXPORT KIT**, **MATCH LEVELS**, and a status line ("Saved to USER 3") |
| **SETUP** | Sample and export folders, **RESCAN**, copy or link samples, **SKIP LOOPS**, **NO DUPLICATES**, **FOLLOW PLAYED PAD**, **MAX SIZE**, and the LOOK |

**TYPE** is what the selected pad draws from: tap the arrows to step through AUTO (the pad's usual sound) and 23 types: KICK, SNARE, RIM, CLAP, HAT, CLOSED HAT, OPEN HAT, TOM, CONGA, PERCUSSION, CRASH, RIDE, CYMBAL, FX, GLITCH, VOX, BASS, SYNTH, STAB, CHORD, LEAD, PAD and OTHER. The pad gets a new sound of that type straight away.

**Kit styles** set what each pad draws from, its pitch, decay and choke, then pick a kit. Locked pads are left alone. **ROLL:** MILD gives up to four pads a new sample; MEDIUM makes a new kit with a little pitch and decay; WILD also mixes up what the pads draw from.

**Exported kits** are normal MPC drum programs (`.xpm`) with the 16 sounds on pads A01-A16, in `PotentialKits/Kits` at the top of the plugin's drive by default. Pads with PITCH, DECAY or GAIN changed are written as you hear them (new WAV files); CHOKE becomes the program's mute groups. With LINK TO ORIGINALS the samples stay where they are and PITCH is set on the pad instead. An MP3 pad is always written as a WAV, because the MPC loads WAVs beside a program.

**Q-Links:** GAIN, PITCH, DECAY and CHOKE of the selected pad (KIT page); the ROLL setting (STYLES page); NO DUPLICATES, SKIP LOOPS, MAX SIZE and copy or link (SETUP page).

**Good to know**

- Reads **WAV** (8, 16, 24 and 32-bit, float, mono or stereo, any sample rate), **AIFF** and **MP3**.
- Samples are sorted by their folder and file names (a folder called `Kicks` holds kicks).
- For a drums-only source, use **MAKE DRUM FOLDER** in MPC TOOLS (1.5.9 or later): it copies the drum and percussion one-shots from every drive into `DRUM ONE SHOTS` on the SD card, already in Kicks, Snares, Hats and so on folders. Then pick `DRUM ONE SHOTS` on the SETUP page and tap **RESCAN**.
- Favourites and rejects carried over from Lucky Dip live in `data/prefs.txt` in the plugin's folder (edit it to undo a reject); the buttons for them are no longer on the screen.
- Pads are one-shots, one voice each. Very long files are cut to 15 seconds.
- Your project remembers the kit, locks and settings, but not the samples themselves: a moved sample shows `!` on its tile.

### 4.5 PotentialNoise

![PotentialNoise](docs/images/potentialnoise.png)

**Mix against the noise.** A calibrated pink noise reference for the pink-noise mixing method, with test tones, octave bands and a speaker check.

**The pink-noise method**

1. Add PotentialNoise to a track and pick a **LEVEL** (-14 dB is a good start).
2. Tap **ON**.
3. Solo each of your tracks with the noise playing, and raise its level until you can only just hear it over the noise.
4. Switch the noise off. You have a balanced starting point for your mix.

The **GUIDE** page walks you through this one tap at a time: **NEXT** switches the noise on and names each part of your beat in turn, with what to listen for. **DRUMS FIRST** or **SAMPLE FIRST** sets the order.

**Controls**

| Control | What it does |
|---|---|
| **NOISE ON / OFF** | Starts off. Fades over about 20 ms, so there are no clicks |
| **LEVEL** | -24, -20, -18, -16, -14 or -12 dB RMS, measured |
| **SIGNAL** | **PINK NOISE**, **TEST TONE** (1 kHz, 100 Hz, 60 Hz or 40 Hz), **OCTAVE BAND** (31 Hz to 16 kHz) or **SPEAKER CHECK** (left, right, in phase, out of phase) |
| **CHANNELS** | MONO or STEREO |
| **RANGE** | FULL (20 Hz and up) or LIMITED (40 Hz to 16 kHz) |
| **RMS / PEAK / CLEAR PEAK** | What it is sending. The meter shows the plugin's own output, not your mix |

**Q-Links:** ON, LEVEL, SIGNAL, TONE, BAND, CHANNELS, RANGE and CLEAR PEAK.

**Good to know**

- It is a starting point, not a finished mix.
- At -12 dB RMS the loudest noise peaks come close to 0 dBFS. -14 dB or lower leaves headroom.

## 5. The effects

### 5.1 PotentialPatina

![PotentialPatina](docs/images/potentialpatina.png)

**Instant character.** Vinyl crackle, tape hiss and wobble, warm drive, bit-crushing, tone and a small room, all set with one big **MAGNITUDE** knob.

**Quick start:** add it as an insert, tap an **ERA**, then turn **AGE** to set how worn it sounds.

| Section | What it adds |
|---|---|
| **MAGNITUDE / MIX / OUTPUT** | Fade the whole effect in and out, blend it with the dry sound, set the level |
| **ERA** | One tap sets all six sections for a sound of its time: Patina, '50s 78, '60s 45, '70s Tape, '80s Sampler, '90s Cassette, AM Radio or VHS |
| **AGE** | Clean to fully worn in one move. 50 % is the sound as set. Tone comes in first, then drive, noise, wobble, and digital and space last |
| **NOISE** | Vinyl crackle, tape hiss or mains hum. **DUCK** turns the noise down while the music plays |
| **WOBBLE** | Tape wow and flutter |
| **DRIVE** | Tape, tube or cassette saturation, level-matched |
| **DIGITAL** | Bit depth and sample-rate reduction |
| **TONE** | One dark to bright tilt knob |
| **SPACE** | A small room |
| **ROLL** | Switches on a random set of sections with random settings |

Each section has its own **ON** switch.

**Presets (MPC's PRESET menu):** Patina, Dusty Vinyl, Worn Tape, Cassette Demo, 12-bit Sampler, Crunchy 8-bit, Old Radio, Basement Room and Clean; then the eight eras; then the four chain presets.

**Q-Links:** MAGNITUDE, AGE, MIX, OUTPUT, TONE, NOISE, WOBBLE and DRIVE, then the section settings on the next bank.

**Good to know:** about 7 ms of delay while it is on, because of how the wobble works. It can be heard on tight layered drums; put Patina on the bus instead. Stereo, 44.1 kHz.

### 5.2 PotentialPunch

![PotentialPunch](docs/images/potentialpunch.png)

**Punch where you want it.** A transient shaper: make every hit snap harder or softer, and pull the tail up or tighten it, with no threshold to set. It reacts to how quickly each hit rises and falls, not to how loud the track is.

**Quick start:** add it as an insert, tap a preset, then fine-tune **ATTACK** and **SUSTAIN**. On a drum bus, start with Punchy Drums or Break Smack.

| Control | What it does |
|---|---|
| **ATTACK** | Up for more snap at the start of each hit (to about +18 dB), down to soften it |
| **SUSTAIN** | Up to bring the tail and room up, down to tighten and dry things out |
| **SPEED** | TIGHT, MEDIUM or SLOW: how long the attack part lasts and how long the tail is followed |
| **FOCUS** | What it listens to: FULL, LOWS (kicks, bass) or HIGHS (snares, hats). The whole sound is always shaped |
| **CLIP** | A soft clipper after the shaper, to catch the extra peaks |
| **AUTO LEVEL** | Evens the loudness in and out, so a punchier setting is not just louder |
| **MIX / OUTPUT** | Blend with the original sound, set the level |
| **ROLL** | A random setting that still sounds good |

**Presets (PRESETS buttons and the PRESET menu):** Punchy Drums, Kick Snap, Snare Crack, Break Smack, Dry Drums, Big Room, Hat Tick, Soft Hits, Bass Pluck, Keys & Guitar, Lush Sustain and Flat; the drum-bus pairs Boom Bap Bus, Lo-Fi Bus, Modern Bus and Glue Bus; the chain presets 90s Stab, Break Bus and Dub Drums.

**Q-Links:** ATTACK, SUSTAIN, MIX, OUTPUT, PRESET, SPEED, FOCUS and CLIP.

**Good to know:** turning ATTACK up raises the peaks, so leave CLIP on or lower OUTPUT if the track gets close to clipping. No added delay. Stereo, 44.1 kHz.

### 5.3 PotentialHeat

![PotentialHeat](docs/images/potentialheat.png)

**Warmth, glue and loudness.** A soft clipper and bus saturator: push drums, breaks or the whole beat into a warm curve that rounds the peaks off instead of clipping.

**Quick start:** add it as an insert, tap a preset, then set how hard it works with **DRIVE**.

| Control | What it does |
|---|---|
| **DRIVE** | How hard the sound is pushed into the curve, 0 to 24 dB |
| **CHARACTER** | SOFT (clean until near the top), TAPE (darker and rounder) or HARD (firmest and loudest) |
| **TONE** | Dark to bright, after the saturation |
| **CEILING** | The most the peaks can reach, -6 to 0 dB |
| **LEVEL MATCH** | Turns the processed sound down by the loudness it gained, so you hear the colour. Off = a loudness clipper |
| **MIX / OUTPUT** | Blend with the original sound (parallel saturation), set the level |
| **ROLL** | A random setting that still sounds good |

**Presets:** Warm Drums, Bus Glue, Break Heat, Loud & Clean, Smash, Tape Glow, Fat Kick, Snare Bite, Bass Warmth, Beat Master, Parallel Crush and Peak Catcher; the drum-bus pairs Boom Bap Bus, Lo-Fi Bus, Modern Bus and Glue Bus.

**Q-Links:** DRIVE, TONE, MIX, OUTPUT, PRESET, CHARACTER, CEILING and LEVEL MATCH.

**Good to know:** the curves run at twice the sample rate inside, so it stays clean when pushed. It adds about 0.3 ms of delay (15 samples); the dry sound in MIX is delayed to match. Stereo, 44.1 kHz.

### 5.4 PotentialGrit

![PotentialGrit](docs/images/potentialgrit.png)

**Seven lo-fi effects in one box, one tap to switch.** Pick one from the pads, shape it with its own knobs, blend it with MIX. Each effect keeps its own settings, and switching crossfades so it won't click.

| Pad | What it does | Controls |
|---|---|---|
| **DUST** | Glue compression with soft saturation and a darker top, plus hiss, crackle and rumble that follow the music, and wobble | COMP, NOISE, WOBBLE |
| **CRUSH** | Bit depth and sample-rate reduction like an early sampler | BITS (2 to 16), RATE (1.5 to 44.1 kHz), TONE |
| **FILTER** | A resonant LOW, BAND or HIGH PASS filter into a saturator | CUTOFF, RESONANCE, DRIVE |
| **KILL** | A DJ-style three-band kill EQ: below 250 Hz, 250 Hz to 3 kHz, above 3 kHz | LOW, MID, HIGH |
| **ECHO** | A dark tape echo locked to your tempo: 1/16, 1/8T, 1/8, 1/8D, 1/4, 1/4D or 1/2. The dry sound always passes through | time pads, FEEDBACK, LEVEL |
| **CHOP** | A tempo gate that follows the song position, 1/4 to 1/32 with triplets | rate pads, DEPTH, PAN |
| **RING** | Ring modulation by a tone that an LFO can sweep two octaves each way | FREQ, SWEEP, SPEED |

**MIX** blends the original (0 %) with the effect (100 %). **OUTPUT** runs from -24 to +12 dB. **ROLL** gives the effect that is on new settings; WILD sometimes picks another effect too.

**Presets (PRESET menu):** Dusty drum bus, Worn record, 12-bit grit, Broken, Next room, Telephone, Dub throw, Stutter, Ping-pong chop and Metal; then the four chain presets.

**Q-Links:** the effect, MIX and OUTPUT, then the knobs of the effects.

### 5.5 PotentialBleed

![PotentialBleed](docs/images/potentialbleed.png)

**The sounds between the chops.** PotentialBleed listens for each hit and adds quiet, synthetic residue behind it, so your chops sound like they came off a real recording with other instruments leaking in. Nothing is delayed.

**Quick start:** put it on a chopped loop or a drum bus and turn **LISTEN** on. Lower **SENSE** until the important hits ring, then raise **BLEED** until you hear it clearly. Turn LISTEN off and bring BLEED back down until the residue sits behind the track.

| Source | Sounds like |
|---|---|
| **WIRES** | Snare wires rattling behind the kick |
| **CABINET** | A bass cabinet's low thump and boom |
| **SOUNDBOARD** | A piano's wooden board ringing |
| **TINES** | Bright struck tines, electric-piano style |
| **STRINGS** | String scrape and ring |
| **CYMBAL** | A soft metallic wash |
| **ROOM** | A small room's low ring and air |
| **BARS** | Vibraphone-like bars |

All eight are generated in the plugin, not recordings.

| Control | What it does |
|---|---|
| **BLEED** | How much leaks in. Small amounts usually sound most real |
| **SENSE** | How hard a hit has to be, -60 to -6 dB |
| **CHANCE** | How often a hit leaks: 100 % is every hit |
| **DECAY** | How long the residue rings, 30 ms to 1.5 s |
| **DUCK / GAPS** | Keeps it down under loud audio; brings it up in the quiet spaces |
| **TONE** | Dark to bright |
| **TICK** | Adds a needle tick on each hit |
| **DRIFT** | A slow wobble, slightly different left and right |
| **LISTEN** | Hear just the residue, to set it up |
| **ROLL** | Picks a new source and shape |

**Presets (PRESET menu):** Wires behind the kit, Bass in the room, Piano lid open, Rhodes shimmer, Guitar scrape, Cymbal wash, Small room, Vibes and Old record; then the four chain presets.

**Q-Links:** BLEED, SOURCE, DECAY, SENSE, CHANCE, DUCK, GAPS and TONE, then TICK, DRIFT, OUTPUT and LISTEN.

### 5.6 PotentialWarp

![PotentialWarp](docs/images/potentialwarp.png)

**Bend time on the beat.** Warp keeps the last few seconds of whatever you feed it and plays them back through a 16-step pattern locked to your sequence.

**Quick start:** add it as an insert, press **Play** on the MPC, then tap a pattern. Try Half Time or Tape Stop on a drum bus, Stutter or Roll Up on a break, Reverse on a vocal chop.

| Move | What you hear |
|---|---|
| **Straight** | The sound as it is |
| **1/2, 1/4 speed** | Drops an octave (or two) and drags behind the beat |
| **Tape stop / start** | Slows to a stop, or spins up from silence |
| **Repeat** | Stutters a slice, from an 8th down to a 64th roll, with triplets |
| **Backwards / Spinback** | The last moment in reverse, or a fast spinback |
| **Silent** | Gates the sound out |

| Control | What it does |
|---|---|
| **PATTERNS** | 16 one-tap pads: Half Time, Tape Stop, Stop Start, Stutter, Roll Up, Beat Repeat, Reverse, Spinback, Gate 16ths, Gate 8ths, Chop, Half End, Slow Mo, Glitch, Wobble and Mangle |
| **LENGTH** | How long the 16 steps last: 1/2 BAR, 1 BAR, 2 BARS or 4 BARS |
| **SWING** | 50 % (straight) to 75 %, MPC style |
| **FADE** | TIGHT or SOFT |
| **WARP** | ON or OFF. Automate it to drop the effect in and out |
| **PLAY** | **ALWAYS** runs with the song. **ONE SHOT** plays a pattern once. **HOLD** plays only while you hold a pad (pads A01 to A16 play patterns 1 to 16) |
| **STOP FILTER** | A low-pass that closes as a Tape Stop runs down |
| **MIX / OUTPUT** | Blend with the original sound, set the level |

**EDIT page:** tap a step, then tap what it does. Steps next to each other with the same move join into one longer move. **ROLL** rolls a new pattern for the pad; **RESET PAD** puts the pad's own pattern back. Every pad's pattern is saved with your project.

**CHAIN page:** tap a slot (1 to 8), then the pattern for it, and turn **CHAIN** on. The slots play in turn, one LENGTH each. (This chains Warp's own patterns. It is not the FX chains of chapter 6.)

**Q-Links:** PATTERN, LENGTH, MIX, OUTPUT, WARP, FADE, SWING and PLAY.

**Good to know:** it follows the song position, in 4/4. When the sequence is stopped it keeps its own time at the project tempo. HOLD needs pads or keys, because the touchscreen does not tell a plugin when your finger lifts.

## 6. Working together: FX chains

### The four chains

Four ready-made chains use Patina, Punch, Grit and Bleed together. Each chain is a preset **with the same name in every plugin of the chain**. Each plugin's preset holds its own part of the sound.

| Chain | For | Inserts, in order |
|---|---|---|
| **Dusty Loop** | Soul, jazz and library loops | **Bleed → Patina → Grit** |
| **90s Stab** | Chopped horns, keys and vocal stabs | **Grit → Punch → Bleed → Patina** |
| **Break Bus** | A boom-bap drum bus | **Punch → Grit → Bleed → Patina** |
| **Dub Drums** | Lo-fi dub drums | **Patina → Punch → Bleed → Grit** |

Dusty Loop does not use Punch, so Punch has no preset by that name.

### Loading a chain, step by step

Using **Break Bus** as the example:

1. Open the inserts of your drum track or drum bus.
2. Add the plugins **in the order shown**: insert 1 PotentialPunch, insert 2 PotentialGrit, insert 3 PotentialBleed, insert 4 PotentialPatina.
3. Open Punch and tap **Break Bus** on its PRESETS buttons.
4. Open Grit, tap the **PRESET** menu at the top of the plugin screen and pick **Break Bus**. Do the same in Bleed and Patina.
5. Match the level with the last plugin's **OUTPUT**, then tweak.

The plugins do not talk to each other. Picking a chain preset in one plugin does not change the others, so pick it in each.

| Plugin | Where to pick the chain preset |
|---|---|
| Patina, Grit, Bleed | The MPC's **PRESET** menu at the top of the plugin screen |
| Punch | Its own **PRESETS** buttons (also in the PRESET menu) |

### What each plugin does in each chain

| Chain | Insert 1 | Insert 2 | Insert 3 | Insert 4 |
|---|---|---|---|---|
| **Dusty Loop** | **Bleed**: SOUNDBOARD, dark, long decay | **Patina**: '60s 45, vinyl noise, tube drive, a little room | **Grit**: FILTER, low pass, gentle | |
| **90s Stab** | **Grit**: CRUSH at 12 bits | **Punch**: attack +30, TIGHT, mix 80 % | **Bleed**: TINES, 60 % chance | **Patina**: '90s Cassette at 60 % mix |
| **Break Bus** | **Punch**: attack +50, sustain +15, CLIP on | **Grit**: DUST, mostly glue | **Bleed**: WIRES, short | **Patina**: '70s Tape at 50 % mix |
| **Dub Drums** | **Patina**: '80s Sampler, 12-bit, no noise | **Punch**: attack +60, sustain -25, LOWS | **Bleed**: ROOM, up in the gaps | **Grit**: ECHO at 1/8 dotted |

The exact values are in the developer reference (chapter 14).

### Why the order matters

- **Punch early.** Shape the hits before compression, crushing or tape, so the shaper sees clean transients.
- **Bleed before Patina.** The bleed is then aged with the sound instead of pasted on top.
- **Echo and chop last.** Grit's ECHO and CHOP, and Warp, should work on the finished sound.
- **One noise source per chain.** Patina's NOISE and Grit's DUST both add noise; two at once turns to mush.
- **Match levels with OUTPUT** before you decide whether you like it.

### Drum-bus pairs: Punch and Heat

**Boom Bap Bus, Lo-Fi Bus, Modern Bus** and **Glue Bus** are in both PotentialPunch and PotentialHeat, made to work together. On your drum bus:

**Punch → Heat**

Pick the same name on the PRESETS buttons of each. Punch shapes the hits, Heat warms them up and catches the peaks.

### Where Heat, Warp and Noise fit

- **Heat** works well as the last insert after any of the four chains, to glue the result and catch peaks. Start with Bus Glue or Peak Catcher. On the master, try Beat Master or Loud & Clean with LEVEL MATCH off.
- **Warp** goes last, so it bends the finished sound: for example **Punch → Grit → Bleed → Patina → Warp**.
- **Noise** is for the mix, once the beat is built: balance the dry tracks, add your chains, then match each chain's level with its OUTPUT.

### Chains of your own

Dial in a sound across several plugins, then save each plugin's part to **the same USER slot number**, for example USER 3 in Punch, Grit and Bleed. To bring the chain back in any project, add the plugins in the same order and tap USER 3 in each. Chapter 7 covers the USER page.

### A worked example

1. **Dig:** in PotentialBreaks, tap ROLL until a break fits.
2. **Shape:** on that track's inserts add Punch, Grit, Bleed and Patina, and pick **Break Bus** in each.
3. **Melody:** in PotentialWax, tap RANDOM TRACK, set KEY, and play the pads. Add Bleed, Patina and Grit to its track and pick **Dusty Loop** in each.
4. **Glue:** send both to a bus with Heat on **Bus Glue**.
5. **Move:** add Warp last on the drum track, PLAY on ONE SHOT, and tap Tape Stop at the end of a phrase.
6. **Balance:** load PotentialNoise on a spare track and follow its GUIDE page.

## 7. USER presets

Patina, Punch, Heat, Grit and Bleed each have a **USER** page with eight slots of your own. The MPC has no "save preset" for plugins like these, so the plugins keep their own.

![The USER page](manual/images/user-page.png)

| To | Do this |
|---|---|
| **Save** | Tap **SAVE** (it lights and reads NOW TAP A SLOT, and stays lit until you tap a slot), then tap a slot, empty or full |
| **Load** | Tap a slot |
| **Cancel a save** | Tap SAVE again |

- A filled slot reads **USER 1** to **USER 8**. An empty one reads **EMPTY**. The slot you loaded or saved last is lit.
- Saving to a filled slot replaces it straight away. There is no confirm step and no undo.
- No typing: slots are known by their number.

**What a slot keeps:** the knobs and switches that make the sound. In Grit, the effect that is on and the settings of all seven. It does not keep the ROLL setting, the look, the built-in preset's highlight (Punch and Heat) or Bleed's LISTEN switch.

**PotentialKits** has the same thing for whole kits: **USER KITS** on its SAVE page keep the samples, locks, sounds, pitch, decay and choke of all 16 pads, in `PotentialSuite Presets/PotentialKits/`.

**Where the slots live:** on the drive the plugin is on, in a folder called `PotentialSuite Presets` beside `VST2 Plugins`, one small text file per plugin. So:

- every project sees the same slots, and so does every copy of the plugin on other tracks;
- updating the plugin keeps them;
- you can back the folder up, or copy it to another MPC's drive.

If the plugin is moved to a different drive, copy the `PotentialSuite Presets` folder to that drive too.

## 8. Troubleshooting

**The plugin is not in my plugin list.**
Restart the MPC: the list only updates when the MPC app starts. Check the right list: Patina, Punch, Heat, Grit, Bleed and Warp are insert effects, not instruments. On MPC OS 3.10.1, check that **External Plugin Support** is on and the folder is inside `VST2 Plugins` at the top of the drive. After a firmware update you may need to switch plugin support on again.

**It shows up twice.**
It was installed two ways. Remove one copy.

**The look did not change.**
A new look shows the next time the plugin loads. Take the plugin off the track and add it again, or load the project again.

**My look went back to Print Plate after an update.**
MPC TOOLS before 1.5.6 replaced the whole plugin folder. Update MPC TOOLS, then pick the look again.

**PotentialKits says No samples found.**
It scans the Expansions and Samples folders at the top of each drive. Pick your sample folder on its SETUP page and tap **RESCAN**. Samples must be WAV, AIFF or MP3.

**PotentialBreaks says NO BREAKS FOUND, or PotentialWax finds no songs.**
Check the folder name and place (chapters 4.1 and 4.2), then tap **RESCAN**. Breaks and Wax both read WAV, FLAC and MP3.

**Pad 1 does not play the first slice or cue.**
The MPC treats a plugin track like a keyboard, so the pads can send different notes. Tap **LEARN**, then hit the pad you want as pad 1.

**The tempo is double or half.**
Put the BPM in the file name (`Funky Drummer 101bpm.wav`). In Wax, use ÷2 or ×2.

**PotentialCrates says to add channels to channels.txt.**
Open `channels.txt` in the plugin's folder (on a computer, or copy it over with MPC TOOLS' File Browser) and add your channels, one a line. PotentialCrates 1.1.2 and later make the file for you, from an old Crate Digger list if it finds one.

**PotentialCrates plays nothing.**
Check the network first, then read the status line: it says why (private, removed, region blocked, rate limited).

**My drums sound slightly late with Patina.**
Patina adds about 7 ms of delay. Put it on the bus so everything moves together.

**A USER slot did not save.**
Update the plugin first: before the versions of 14 November 2026 (Patina 1.5.1, Punch 1.5.1, Heat 1.4.1, Grit 1.1.2, Bleed 1.1.1, Kits 1.1.0) one tap on SAVE could switch it straight back off. Then wait three seconds after the plugin or project loads before tapping slots: taps are ignored for a moment so that loading a project cannot change a slot. Check that the drive is not full or read-only.

**The chain does not sound like I expected.**
Check the insert order, check the same preset name is picked in every plugin of the chain, and match the level with OUTPUT.

**Something else.**
Each plugin's GitHub page has an issue form. Short reports are fine: "the PITCH knob does nothing on my Live II" is a perfect bug report.

## 9. Compatibility and CPU

### Tested on

{{TESTED}}

"Tested" means someone ran that plugin on that MPC and reported it. Every plugin is built for first-generation (32-bit) and second-generation (64-bit) MPCs; the 64-bit builds pass Akai's own checks. If you try a plugin on another model, please report how it went on its GitHub page.

### CPU

Measured on an MPC One (MPC OS 3.10.1) with the Open MPC bench, from MPC TOOLS' **TEST** button, as a share of one audio block. Under 15 % at the 99th percentile and 50 % in the worst block is a PASS: several copies run alongside a normal project.

{{CPU}}

# Part 2: Developer reference

## 10. How a plugin is put together

Each plugin is a native **VST2** shared library for ARM Linux, loaded by MPC's own plugin host, with a native MPC screen skin. All ten are built with the open [Open MPC kit](https://github.com/open-mpc/mpc-vst-plugins) (`mpc-vst-plugins`, MIT).

| Layer | Where | What it does |
|---|---|---|
| **Engine** | `src/<name>.c` in the plugin's repo | The DSP and all behaviour. Plain C, no dependencies beyond libm. Exposes `mpc_engine()` |
| **Wrapper** | The kit's `wrapper/vst2_wrap.c` | Turns the engine into a VST2 plugin: parameters, state chunk, MIDI, audio blocks |
| **Parameters** | `vst/params.json`, `vst/params.lock` | The parameter list, in VST index order. The kit generates `params.h` from it |
| **Layout** | `vst/layout.base.conf` | Where every control sits on each tab |
| **Looks** | `shared/looks/`, `vst/skin/looks/` | Turns the one layout into three skins and merges them |
| **Shared headers** | `shared/` | The same files in every repo: look switch, USER slots, MANUAL page |

### Repositories

| Repository | Holds |
|---|---|
| `PotentialBreakfast/mpc-vst-potential<name>` | One plugin each: bleed, breaks, crates, grit, heat, kits, noise, patina, punch, warp, wax |
| `PotentialBreakfast/potential-plugins` | The overview page, this manual, the suite FAQ and install guide |
| `PotentialBreakfast/mpc-tools` | MPC TOOLS, the Windows and Mac companion app (Go) |
| `open-mpc/mpc-vst-plugins` | The build kit, and the Open MPC catalogue registry |

### Layout of a plugin repo

```
src/<name>.c              the engine
vst/vst.json              name, vendor, uid, version, defines, sources
vst/params.json           parameters, in index order
vst/params.lock           the keys in order: append-only
vst/presets.json          presets for the MPC's PRESET menu (some plugins)
vst/layout.base.conf      the layout, edited by hand
vst/layout*.conf          generated: one per look
vst/manual.txt            the on-screen manual
vst/skin/looks/           stylesheets, fonts and plugin.json (accent colours, hero knobs)
shared/looks/             make_looks.py, build_looks.py, potential_theme.h
shared/potential_user.h   USER slots (the five effects)
shared/potential_manual.h MANUAL page
shared/akai/akai_pack.py  Akai's plugin-folder format
tests/                    engine tests (some plugins)
.github/workflows/        release.yml
.github/release-notes/    v<version>.md: adding one triggers a release
tested.json               devices the plugin has been reported working on
```

## 11. The engine interface

The engine implements the kit's `wrapper/engine.h`. Audio is **44100 Hz, interleaved int16 stereo, in blocks of up to 128 frames**.

```c
typedef struct {
    void *(*create)(const char *data_dir);
    void (*destroy)(void *inst);
    void (*midi)(void *inst, const uint8_t *msg, int len);
    void (*set_param)(void *inst, const char *key, const char *val);
    int  (*get_param)(void *inst, const char *key, char *buf, int buf_len);
    void (*render)(void *inst, int16_t *out_lr, int frames);            /* instruments */
    void (*process)(void *inst, const int16_t *in_lr, int16_t *out_lr, int frames);  /* effects */
    void (*song_pos)(void *inst, double ppq, double bpm, int playing, int frames);
} mpc_engine_t;
const mpc_engine_t *mpc_engine(void);
int mpc_engine_changed(void *inst, char *key, int len);   /* optional, weak */
```

| Function | Notes |
|---|---|
| `create(data_dir)` | `data_dir` is the plugin's own folder (`"MODULE_SUBDIR": "\".\""` in `vst.json`). The looks, USER slots and file-based plugins find their files from it |
| `set_param(key, val)` | Values arrive as text. Options arrive as their index. A trigger arrives as 1 |
| `get_param(key, buf)` | Returns the number of characters written, 0 for an unknown key |
| `process` / `render` | The wrapper never calls two functions at once for one instance, but calls may come from the screen and audio threads |
| `song_pos` | With `"HAS_SONG_POS": 1`: song position in beats, tempo and transport once per block. Grit, Warp and Crates use it |
| `mpc_engine_changed` | The engine names parameters it changed itself (a ROLL, a USER slot load), one key per call, so MPC's screen follows |

### Keys the wrapper asks for

| Key | Meaning |
|---|---|
| `state` | The whole state as text, `key=value;key=value;...`. Saved in the project and restored with `set_param("state", ...)`. Restoring sets values directly, without re-applying a preset |
| `display_rev` | With `HAS_DISPLAY_REV`: a counter the engine bumps when on-screen text changed. The four plugins with a preset strip set `DISPLAY_REV_NO_UPDATE`, because a screen refresh closes MPC's PRESET menu |
| `<key>_display` | With `"dynamic_display": true` on a parameter: its value text (Grit's rates and times) |
| `<key>_N`, `<key>_N_on` | A `list` widget's rows: the text of row N and whether it is lit |
| `<key>:<n>` | With `"programs"` in `vst.json`: the name of preset n |

### Conventions across the suite

- **Parameter 0** is never a live control: MPC flicks parameter 0 when the transport stops.
- **Triggers** (`"type": "trigger", "momentary": true`) fire on a press. The wrapper debounces them.
- **`dice`** is the ROLL trigger and **`wild`** its MILD / MEDIUM / WILD option.
- **`theme`** is the LOOK selector. It is not in `state`.
- **`manual_page`**, **`user_save`** and **`user_1` to `user_8`** are not in `state`.
- A project reload replays parameter values, so anything that acts on a tap ignores taps for 3 seconds after `create` and after a `state` restore.
- Presets changed "only on a change": re-sending the same preset number keeps the user's tweaks.

## 12. Parameters

`vst/params.json` lists the parameters in VST index order. **The list is append-only**: `vst/params.lock` holds the keys in order and the build fails if an existing key moves, because saved projects and MPC automation address parameters by index.

| Field | Meaning |
|---|---|
| `key` | The name used by the engine, the layout and `state` |
| `name` | The name MPC shows |
| `min`, `max`, `default`, `unit` | A continuous parameter |
| `options` | A list of choices; the value is the index |
| `type` | `trigger`, `readout`, `slot` and others; absent for an ordinary control |
| `momentary` | A trigger that springs back |
| `display: "string"` | The parameter's text comes from the engine |
| `live`, `poll` | Whether the text is re-read while the page is open |
| `dynamic_display` | The value text comes from `<key>_display` |
| `legacy` | Kept only so old projects load |

Appendix A lists every parameter of every plugin.

## 13. Layout and looks

`vst/layout.base.conf` is the only layout edited by hand. The screen is 1280 × 800; the plugin area starts at y = 100.

```
[tab PUNCH]
frame x=14 y=100 w=1252 h=604 title="PotentialPunch"
knob cx=220 cy=330 r=59 label="ATTACK" key=attack
enum_v cx=721 cy=600 label="SPEED" key=speed sw=124
toggle cx=1040 cy=514 label="CLIP" key=clip
button cx=640 cy=250 label="SAVE" label_on="NOW TAP A SLOT" key=user_save w=440 h=96
list x=40 y=330 w=1200 h=316 cols=4 rows=2 th=150 gap=16 key=user mark=1
qlinks "PUNCH" = attack,sustain,mix,output,preset_pick,speed,focus,clip
```

| Widget | Use |
|---|---|
| `frame`, `text` | A titled panel; free-standing text |
| `knob`, `slider_v`, `slider_h` | Continuous controls. A knob's radius must be 59 or less (MPC garbles taller filmstrips) |
| `toggle`, `button` | A switch; a trigger or latching button with `label_on=` |
| `enum_h`, `enum_v` | One button per option |
| `readout`, `list` | Engine text; rows of engine text that can be tapped |
| `picture`, `meter`, `art` | Images that follow a parameter, or sit in the background |
| `qlinks "TAB" = a,b,c` | The Q-Link assignments of a tab, in banks |
| `when=<param>:<option>` | On any line: show the widget only in that mode (Grit's effect panels, the MANUAL pages) |

A button key used on several pages shares one image, so it must be the same size everywhere.

### The looks

`shared/looks/make_looks.py vst` writes `layout.conf` (Print Plate), `layout.rack.conf` (Night Rack) and `layout.block.conf` (Colour Block) from the base layout, adding each look's palette and the plugin's own colour from `vst/skin/looks/plugin.json`:

```json
{"name": "PotentialPunch",
 "accent": {"print": "2d5be3", "rack": "7c9bff", "block": "6f93ff"},
 "hero": ["attack", "sustain"]}
```

It also appends the **MANUAL** tab (from `vst/manual.txt`) and the **SETTINGS** tab with the LOOK selector. `shared/looks/build_looks.py vst <kit>` builds all three with the kit's `gen_vst.py` and merges them with `make_skinset.py`.

**Switching looks.** MPC reads a skin only when a plugin loads. `shared/looks/potential_theme.h` copies the chosen look's `TUI.json` and Q-Link maps into `Plugin Skins/` and writes the look's name to `Plugin Skins/ui.txt`; the new look shows on the next load.

### The MANUAL page

`vst/manual.txt` is the on-screen manual:

```
= START                 a page; START is its button
# Heading
Body text, wrapped for you.
- A bullet
```

`make_looks.py` wraps the text, stops the build if a page does not fit (13 lines under the heading), and draws each page with `when=manual_page:<n>`. The page names must match the options of the parameter `manual_page`, in order. `shared/potential_manual.h` holds the page number:

```c
pm_manual_t manual;                                       /* in the instance */
if (pm_set(&s->manual, key, val)) return;                 /* in set_param() */
n = pm_get(&s->manual, key, buf, buf_len); if (n) return n;   /* in get_param() */
```

## 14. Presets and USER slots

### Built-in presets

- **Punch, Heat, Warp** keep presets in the engine (a table in the C source) and expose them through `"programs": {"param": "preset_pick"}` in `vst.json`, so the same list is on the page and in MPC's PRESET menu.
- **Patina, Grit, Bleed** use `vst/presets.json`, which the kit turns into MPC preset files:

```json
{"presets": [
  {"name": "Break Bus",
   "values": {"fx": "DUST", "dust_comp": 75, "dust_noise": 15, "dust_wobble": 10, "mix": 100, "output": 0}}
]}
```

Values are applied in file order. Patina's presets set `era` first, because picking an era sets the six sections, and the values after it then overwrite them.

**Chain presets** are ordinary presets that share a name across plugins. To add a chain, add a preset of the same name to each plugin in it and document the insert order.

### Chain preset values

{{CHAIN_VALUES}}

### USER slots

`shared/potential_user.h` (Patina, Punch, Heat, Grit, Bleed).

- **File:** `<drive>/PotentialSuite Presets/<Plugin>.txt`, two folders up from the plugin's folder, so beside `VST2 Plugins`. Written through a `.tmp` file and a rename.
- **Format:** one line per filled slot, `<slot number><TAB><state text>`. The state text is the engine's own `state`, without the keys the engine drops (`wild`, `dice`, `preset_pick`, Bleed's `listen`).
- **Parameters:** `user_save` (OFF / ON, the SAVE button) and `user_1` to `user_8` (`"type": "slot"`, rows of the USER page's `list`). `user_N` returns `USER N` or `EMPTY N`; `user_N_on` lights the slot loaded or saved last.
- **Taps:** a list row toggles on every tap, so any `set_param("user_N")` is a tap. Taps are ignored for 3 seconds after `create` and after a project's `state` arrives, and a second tap on one row within 400 ms is ignored.
- **Save:** `get_param("state")`, drop keys, write the file, clear `user_save` and report it through `mpc_engine_changed`.
- **Load:** re-read the file (another copy of the plugin may have saved), `set_param("state", slot)`, then report every sound parameter so MPC's controls move.
- **Without a folder** (the computer test, a desktop host) the slots live in memory.

```c
int n = 0, r = pu_set(&s->user, key, val, &n);
if (r == PU_SAVE) { char st[PU_STATE_MAX]; get_param(s, "state", st, sizeof st);
                    pu_store(&s->user, n, st, "dice,wild,preset_pick"); }
else if (r == PU_LOAD) { /* restore pu_slot(&s->user, n), then mark every control changed */ }
if (r) return;
```

`shared/test_user.c` tests the slots against the real engine on the computer.

## 15. Files on the MPC

| Path | Written by | Holds |
|---|---|---|
| `<drive>/VST2 Plugins/PotentialBreaks - VST - <Plugin>/` | Installer | The plugin: `.so` files, `plugin.json`, `mpc-plugin.json`, `Plugin Skins/` |
| `.../Plugin Skins/ui.txt` | Plugin | The chosen look: `print`, `rack` or `block` |
| `.../Plugin Skins/print`, `rack`, `block` | Installer | Each look's `TUI.json` and Q-Link maps |
| `<drive>/PotentialSuite Presets/<Plugin>.txt` | Plugin | USER slots |
| `<drive>/DRUM BREAKS LIBRARY/` | You | Breaks' library |
| `<drive>/PotentialBreaks/folders.txt`, `kit-folder.txt`, `Kits/` | Breaks, you | Extra library folders, your programs folder, saved kits and grooves |
| `<drive>/MELODIC LIBRARY/` | You | Wax's library |
| `<drive>/PotentialWax/folders.txt`, `kit-folder.txt`, `Kits/` | Wax, you | The same for Wax, and saved pads |
| `<drive>/PotentialKits/Kits/` | Kits | Exported drum programs |
| `<plugin folder>/data/` | Kits, you | PotentialKits' sample list, favourites and rejects (`prefs.txt`), optional `potentialkits.conf`. Kept by updates |
| `<drive>/PotentialSuite Presets/PotentialKits/` | Kits | USER KITS |
| `<plugin folder>/channels.txt` | You | Crates' YouTube channels. Listed as `user_data` in `mpc-plugin.json`, so installers keep it |
| `<drive>/Potential-backup-<date>/` | MPC TOOLS | Old copies moved aside by an update |
| `<SD card>/DRUM ONE SHOTS/` | MPC TOOLS | Drum and percussion one-shots from every drive, in Kicks, Snares, Claps, Hats, Cymbals, Toms and Percs (MAKE DRUM FOLDER). `.drumsort/index.tsv` remembers what was checked |

## 16. Building, testing and releasing

### Build

You need Linux or WSL, `gcc`, Python 3, `pip install ziglang pillow playwright` and `python -m playwright install chromium` (the skins are drawn by the kit's browser renderer). No Docker.

```
git clone https://github.com/open-mpc/mpc-vst-plugins
git clone https://github.com/PotentialBreakfast/mpc-vst-potentialpunch
K=$PWD/mpc-vst-plugins; P=$PWD/mpc-vst-potentialpunch

# 1. the three looks' skins and params.h
python3 $P/shared/looks/build_looks.py $P/vst $K

# 2. test on your computer (x86, AddressSanitizer)
bash $K/tools/test_port.sh $P/vst/vst.json

# 3. build for the MPC: 32-bit, then 64-bit
cd $P && python3 -m ziglang cc -target arm-linux-gnueabihf.2.31 -mcpu=cortex_a9 -mfpu=neon -O2 -fPIC -shared \
  -fvisibility=hidden -std=gnu11 -ffast-math -I src -I vst/build -I $K/wrapper src/punch.c $K/wrapper/vst2_wrap.c \
  -lm -lpthread -Wl,--no-undefined -Wl,-s -o vst/build/potentialpunch.so
python3 -m ziglang cc -target aarch64-linux-gnu.2.31 -mcpu=cortex_a55 ... -o vst/build/potentialpunch-arm64.so
```

Each repo's `BUILD.md` and `.github/workflows/release.yml` have the exact, current commands, including the kit commit the repo is pinned to.

### Test

| Check | How |
|---|---|
| Host test | `tools/test_port.sh <vst.json>`: loads the plugin in a test host on the computer with AddressSanitizer |
| Engine tests | `tests/test_<name>.c` where present; `shared/test_user.c` for the USER slots; `shared/looks/test_theme.c` for the look switch |
| Skin preview | The kit's `tools/studio.py preview <Plugin Skins folder> -o page_%d.png` draws every tab and mode without an MPC |
| Catalogue check | `tools/catalog_check.py dist/*.zip --catalog` |
| On the MPC | MPC TOOLS **TEST** runs the Open MPC bench (CPU per audio block) and a load test; **RECORD ON GITHUB** adds the result to `tested.json` |

### Release

Versions are `major.minor.patch`, stored in `vst.json` as one number: 1.5.0 is `1500`, 1.2.3 is `1203`.

1. Bump `"version"` in `vst/vst.json`.
2. Add `.github/release-notes/v<version>.md`.
3. Update `CHANGELOG.md` and the version in `README.md`.
4. Push to `main`.

The release workflow builds every notes file that has no release yet: skins, host test, ARM builds, zips, Akai's converter and checker, catalogue check, then publishes the release with the tag on that commit. MPC TOOLS can do steps 1 to 4 for you with **NEW VERSION**.

| File | For |
|---|---|
| `<Name>-<ver>-akai-vst2.zip` | MPC OS 3.10.1+: Akai's plugin-folder format, 32-bit and 64-bit |
| `<Name>-<ver>-mpc-sdk.zip` | The same folder in the Open MPC catalogue's 3.10.1 format. MPC TOOLS installs this one |
| `<Name>-<ver>-mpc-armv7.zip` | First-generation MPCs with the community set-up |
| `<Name>-<ver>-mpc-aarch64.zip` | Second-generation MPCs with the Open MPC Plugin Manager |
| `SHA256SUMS` | Checksums |

### The Open MPC catalogue

Each plugin has an entry in the registry at `open-mpc/mpc-vst-plugins`, `catalog/plugins/<id>.json`, and MPC TOOLS has one in `catalog/apps.json`. The registry's `tools/catalog_build.py --check-registry` validates entries. Screenshot links are pinned to a commit.

### MPC TOOLS and the plugins

MPC TOOLS finds the newest release of each plugin from its GitHub releases page, downloads the `-mpc-sdk.zip`, copies the folder to `<drive>/potential-update/` over SSH and swaps it into `VST2 Plugins`. The old copy is moved to `Potential-backup-<date>`. From 1.5.6 the swap carries the package's `user_data` files and the chosen look into the new copy.

## 17. Credits and licences

- All eleven plugins and MPC TOOLS are **MIT** licensed: free to use, change and share. Third-party parts keep their own licences (`THIRD_PARTY.md` in each repo).
- Built with the **Open MPC kit**, [mpc-vst-plugins](https://github.com/open-mpc/mpc-vst-plugins) (MIT), started by sd88me: the plugin wrapper, skin builder and installer.
- **PotentialKits** is a fork of [Lucky Dip](https://github.com/sd88me/mpc-vst-luckydip) by sd88me (MIT), whose kit engine came from his schwung-kit-builder and force-kit-builder.
- **PotentialCrates** is a fork of [Crate Digger](https://github.com/sd88me/mpc-vst-cratedigger) by sd88me, with a streaming engine built on Charles Vestal's [schwung-webstream](https://github.com/charlesvestal/schwung-webstream). Its engine bundle carries yt-dlp (Unlicense), ffmpeg (GPL-3.0) and Python (PSF).
- **PotentialGrit** is inspired by DustBox and **PotentialBleed** by Ghost Sample. **PotentialWax** is inspired by the way Serato Sample works, **PotentialWarp** is in the spirit of Gross Beat, and **PotentialPatina** by the "retro colour" style of plugin. All are independent designs with their own code, not affiliated with those products.
- Audio file reading by [dr_libs](https://github.com/mackron/dr_libs) by David Reid.
- Fonts: Jost, DM Mono, IBM Plex Sans Condensed, IBM Plex Mono, Bricolage Grotesque and Space Mono (SIL Open Font License 1.1).
- Made with help from Claude (Anthropic).

Not affiliated with or endorsed by Akai Professional or inMusic. MPC is their trademark.

# Appendix A: Parameter reference

The **#** column is the VST parameter index. Rows marked *text* are engine text, *trigger* fire on a press, and *slot* are rows of a list.

{{ALL_PARAMS}}
