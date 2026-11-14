# Changelog

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
