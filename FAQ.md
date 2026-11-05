# FAQ and troubleshooting

Can't find your answer? [Ask in an issue](../../issues/new/choose). There are no silly questions.

## Installing

**The plugin isn't in my plugin list.**
- **Restart:** the list only updates after the MPC app restarts, so turn the MPC off and on if in doubt.
- **The wrong list:** PotentialPatina is an **effect**, so look in the **insert effects**, not the instruments.
- **The MPC isn't set up for third-party plugins yet:** see [Before you start](INSTALL-GUIDE.md#before-you-start-is-your-mpc-ready-for-plugins).
- **After a firmware update:** an update can switch plugin loading off again. Redo the set-up from [Open MPC](https://open-mpc.github.io/).

**It's in the list but only shows "Load Plugin" or a plain page of sliders.**
The plugin folder is on a drive the MPC can't run programs from, or isn't in a folder the MPC knows about. Reinstall with the [MPC Plugin Installer](INSTALL-GUIDE.md#way-1-the-easy-way-the-mpc-plugin-installer-app), which picks a suitable place.

**The installer says "No space left on device" or "settings edit failed".**
The MPC's internal storage is full. A common cause is the MPC's own cache of plugin graphics in `/var/tmp/filmstrips`, which can grow to gigabytes. With the MPC app stopped, deleting `/var/tmp/filmstrips/temp_*` is safe, because the MPC makes new ones when it needs them.

**Which file do I download?**
The one ending **`-mpc-armv7.zip`** from [Releases](../../releases). The "Source code" links are for developers.

**Will it work on my MPC One+, Live III or Key 37?**
Not yet. Those are Gen2 machines, and there's no Gen2 build. [Let me know](../../issues/new?template=idea.yml) if you'd use one, because it helps me decide what to do next.

## PotentialBreaks

**It says "NO BREAKS FOUND".**
It looks for breaks in three places:
- a folder called `DRUM BREAKS LIBRARY` at the top of a drive
- any folder at the top of a drive with **break** in its name (`My Breaks`, `Breakbeats`…)
- folders listed in `PotentialBreaks/folders.txt`

Rename or move your folder, then tap **RESCAN**. Only **WAV** and **FLAC** files are read, not MP3 or AIFF.

**Pad 1 doesn't play the first slice, or the pads play the wrong slices.**
The MPC treats a plugin track like a keyboard, so depending on the pad bank and octave the pads send different notes. Tap **LEARN**, then hit the pad you want as pad 1. The STATUS line shows the last note received.

**The tempo is wrong, or it's twice or half as fast.**
Put the tempo in the file name, for example `Funky Drummer 101bpm.wav`, and it will be used exactly. Without it, the tempo is guessed between 80 and 160 BPM.

**What's the difference between RE-PITCH and STRETCH?**
- **RE-PITCH** plays the break faster or slower to match your tempo, so the pitch moves with it, like an old sampler.
- **STRETCH** keeps the pitch and trims each slice to fit your tempo. Classic chopped breaks; at slower tempos you'll hear little gaps between hits.

**Where does SAVE GROOVE put the MIDI file?**
In a `PotentialBreaks` folder on the drive your breaks are on. Load it into a sequence on the same track to play the break in its original order.

**It only used part of my long file.**
Files longer than 3 minutes are only read for their first 3 minutes, to keep the MPC's memory safe. For whole albums, cut them into shorter files.

## PotentialPatina

**It's too much, or not enough.**
Turn the big **MAGNITUDE** knob, which scales everything at once, or use **MIX** to blend in the original sound. The presets start subtle.

**My drums sound slightly late with it on.**
PotentialPatina adds about 7 ms of delay. On tight layered drums, try putting it on a bus instead, or switch it off on the parts that need to stay locked.

## Everything else

**Is it free? Can I use it in my music?**
Yes and yes. Everything here is MIT-licensed, so use it however you like, commercially too. Credit is appreciated but not required.

**Can I suggest an idea or report a bug?**
Please do: [ideas](../../issues/new?template=idea.yml) · [bugs](../../issues/new?template=bug.yml).
