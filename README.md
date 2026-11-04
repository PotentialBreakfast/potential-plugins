# Potential Plugins

Free plugins for **Akai MPC standalone hardware**, made by **PotentialBreaks** for my own beat-making and shared for anyone who wants them.

They run inside the MPC like its own instruments and effects. Each one has its own touchscreen page, works with Q-Links and saves with your project, with no computer needed while you play.

| Plugin | What it is | Latest |
|---|---|---|
| [**PotentialBreaks**](plugins/potentialbreaks/README.md) | Drum-break slicer: a random break from your own library on 16 pads, chopped and matched to your tempo | 1.1.1 |
| [**PotentialPatina**](plugins/potentialpatina/README.md) | Retro-colour insert effect: vinyl and tape noise, wobble, drive, bit-crush, tone and room in one box | 1.0.2 |

On the way: **PotentialWax**, a sampler for chopping melodic loops and whole songs.

![PotentialBreaks on the MPC touchscreen](docs/images/potentialbreaks.png)

---

## Will it work on my MPC?

| | |
|---|---|
| **Tested on** | MPC One (first generation) with HAKAI 3.10 firmware |
| **Should work on** | Other first-generation (Gen1, 32-bit) standalone MPCs running **MPC OS 3.x**: One, Live, Live II, X |
| **Not yet** | Gen2 hardware (MPC One+, Live III, Key 37, XL and others): no build yet. **MPC OS 2.x**: not supported |
| **You need** | A way to install third-party plugins on your MPC (see below) |

The **Should work on** row is untested: these plugins have only been tried on one MPC so far. If you try one on another model, please [tell me how it went](../../issues/new/choose). That's how this table gets filled in.

---

## Install

Pick a plugin from [**Releases**](../../releases) and download its `-mpc-armv7.zip`.

**The easy way:** install it with the [Open MPC](https://open-mpc.github.io/) installer app, which also lets you remove it again later.

**Over SSH** (if you have root SSH access to your MPC):
```
scp PotentialBreaks-1.1.1-mpc-armv7.zip root@<your-mpc-ip>:/tmp/
ssh root@<your-mpc-ip>
cd /tmp && unzip -o PotentialBreaks-1.1.1-mpc-armv7.zip
sh PotentialBreaks-1.1.1/install.sh -t /media/<your-drive>/Synths
```
- **Save your project first.** The installer restarts the MPC.
- It backs up the MPC's settings before changing anything, and `uninstall.sh` removes the plugin cleanly.
- `INSTALL.md` inside each zip has the full steps, including a manual install.
- Unzip **on the MPC**, not on Windows. Windows unzipping loses the permissions the installer needs.

---

## Things to know (limitations)

I'd rather you know these up front:

- **Made by one person, tested on one MPC.** They work well for me, but you may hit something I haven't. Please tell me if you do.
- **Gen1 only for now.** There's no build for Gen2 machines yet.
- **Plugins must be allowed to run on your MPC.** Stock firmware doesn't load third-party plugins until it has been set up for them (see [Open MPC](https://open-mpc.github.io/)). After a firmware update, you may have to redo that setup.
- **Each plugin's page lists its own limits.** See [PotentialBreaks](plugins/potentialbreaks/README.md#limitations) and [PotentialPatina](plugins/potentialpatina/README.md#limitations).
- **No warranty.** These are free and come as they are. Back up your projects. I can't be responsible for lost work.

---

## Ideas, bugs and criticism: all welcome

I make these for myself first, and I want them to be good for everyone else too, so **please tell me what you think**, including what you don't like.

- 💡 **Got an idea?** [Suggest a feature or a new plugin](../../issues/new?template=idea.yml).
- 🐛 **Something broken?** [Report a bug](../../issues/new?template=bug.yml).
- 💬 **Just want to say how it's going?** Any of the above is fine. Short messages are great.

You don't need to be technical. "The PITCH knob does nothing on my Live II" is a perfect bug report. See [CONTRIBUTING.md](CONTRIBUTING.md) for more, including how to send code.

---

## Credits

- Built on **[mpc-vst-plugins](https://github.com/sd88me/mpc-vst-plugins)** by sd88me (MIT), the open kit that makes MPC plugins possible: the plugin wrapper, skin builder and installer.
- Audio file reading by **[dr_libs](https://github.com/mackron/dr_libs)** (dr_wav and dr_flac) by David Reid (public domain / MIT-0).
- The look matches the dark theme I made for sd88me's Lucky Dip.
- Made with help from Claude (Anthropic).

Not affiliated with or endorsed by Akai Professional or inMusic. MPC is their trademark.

## Licence

[MIT](LICENSE): free to use, change and share, including in your own projects. Third-party parts keep their own licences ([THIRD_PARTY.md](THIRD_PARTY.md)).
