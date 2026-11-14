# Changelog

## 2026-11-14 – the manual brought up to date
- **Versions covered:** Breaks 1.7.0, Wax 1.8.0, Crates 1.1.2, Kits 1.1.1, Patina 1.5.1, Punch 1.5.1, Heat 1.4.1, Grit 1.1.2, Bleed 1.1.1 and MPC TOOLS 1.5.9; tested devices and CPU figures from the newest results.
- **PotentialKits (4.4):** rewritten for the 1.1 pages (KIT, STYLES, SAVE, SETUP). The POOL and DIP pages are gone: **TYPE** on the KIT page picks what a pad plays; PLAY, CLEAR, FAVOURITE and REJECT are off the screen. New KIT page picture.
- **MP3:** Breaks and Kits read MP3 now (Wax already did); Kits exports an MP3 pad as a WAV.
- **Wax:** the WAVE page's moving playhead. **Crates:** `channels.txt` is made for you (from an old Crate Digger list if there is one).
- **USER presets and troubleshooting:** SAVE stays lit until you tap a slot; which versions fixed "a USER slot did not save"; Crates' channels message.
- **MPC TOOLS 1.5.9:** MAKE DRUM FOLDER (`DRUM ONE SHOTS` on the SD card) in chapter 4.4 and the files table; README mentions it and the Remote buttons.

## 2026-11-14 – PotentialKits
- **PotentialKits** joins the suite: a random drum-kit builder, forked from sd88me's Lucky Dip, with KIT STYLES, ROLL, pitch, decay and choke per pad, USER kits and the three looks. Added to the plugin table, the screenshots, the install notes, the credits and the manual (chapter 4.4).

## 2026-11-14 – the PotentialSuite manual
- **[MANUAL.md](MANUAL.md)** and **[PotentialSuite-Manual.pdf](PotentialSuite-Manual.pdf)**: a user guide (every plugin, the FX chains and their load orders, USER presets, troubleshooting) and a developer reference (engine interface, parameters, layout and looks, presets, files on the MPC, building and releasing, and every parameter of every plugin).
- Built by `manual/build.py` from `manual/src.md` and the plugin repos, so versions, parameters and chain values come from the source.
- README and FAQ brought up to date: MPC OS 3.10.1 install, Gen2 builds, tested MPCs.

## 2026-11 – first public release

### PotentialBreaks 1.1.1
- First public release.
- Random break, PREV, NEXT and LIBRARY; long tracks are cut to random 1, 2 or 4 bar loops.
- Tempo read from the file name, or detected; Re-pitch and Stretch modes.
- Chop: Grid 8, Grid 16 (moved onto hits) or Hits; SHUFFLE and ORDER; CHOKE, PITCH and VOLUME.
- LEARN for pad mapping; PLAY LOOP; SAVE GROOVE (a MIDI file of the slice order).
- Finds breaks in `DRUM BREAKS LIBRARY`, in any top-level folder with "break" in its name, or in folders listed in `PotentialBreaks/folders.txt`; RESCAN.
- Only reads the first 3 minutes of a file, to protect the MPC's memory.

### PotentialPatina 1.0.2
- First public release.
- Noise, Wobble, Drive, Digital, Tone and Space sections with a MAGNITUDE knob.
- 9 presets, set subtle.
