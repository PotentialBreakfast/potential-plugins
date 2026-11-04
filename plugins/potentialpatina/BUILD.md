# Building PotentialPatina

You don't need to build it to use it: download the zip from [Releases](../../../releases). This is for people who want to change it.

It's built with sd88me's [mpc-vst-plugins](https://github.com/sd88me/mpc-vst-plugins) kit, and needs Linux or WSL, `gcc`, Python 3 and `pip install ziglang`. You don't need Docker.

```
git clone https://github.com/sd88me/mpc-vst-plugins
git clone https://github.com/PotentialBreakfast/potential-plugins
cd mpc-vst-plugins && git apply ../potential-plugins/shared/mpc-vst-plugins-label-fit.patch && cd ..
K=$PWD/mpc-vst-plugins; P=$PWD/potential-plugins/plugins/potentialpatina

# 1. skin + params.h (button labels in a real font, like the released version)
export SHADOW_TITLE_FONT=$K/tools/html_art/fonts/TitilliumWeb-SemiBold.ttf
mkdir -p $P/vst/build
gcc -O2 -I $K/tools/vendor/force-shadow/tools -o $P/vst/build/shadow_art $K/tools/shadow_art.c -lm
python3 $K/tools/gen_vst.py $P/vst/vst.json

# 2. test on your computer (x86, with AddressSanitizer)
bash $K/tools/test_port.sh $P/vst/vst.json

# 3. build for the MPC (Gen1, 32-bit ARM)
cd $P && python3 -m ziglang cc -target arm-linux-gnueabihf.2.31 -mcpu=cortex_a9 -mfpu=neon -O2 -fPIC -shared \
  -fvisibility=hidden -std=gnu11 -ffast-math  -I vst/build -I $K/wrapper src/patina.c $K/wrapper/vst2_wrap.c \
  -lm -lpthread -Wl,--no-undefined -Wl,-s -o vst/build/potentialpatina.so

# 4. the installer zip
python3 $K/tools/release.py --so vst/build/potentialpatina.so --skin "vst/build/skin/PotentialBreaks - VST - PotentialPatina" \
  --entry vst/build/pluginlist-entry.xml --version X.Y.Z --id potentialpatina --repo PotentialBreakfast/potential-plugins --license MIT -o dist
python3 $K/tools/catalog_check.py dist/*.zip --catalog
```

**Notes**
- **Parameters are append-only.** `vst/params.lock` keeps saved projects working, so add new parameters at the end of the list.
- **The look:** the `theme_*` lines at the top of `vst/layout.conf` come from [`shared/potential-theme.conf`](../../shared/potential-theme.conf), the dark theme all Potential plugins share.
- **The patch:** `shared/mpc-vst-plugins-label-fit.patch` makes button labels shrink to fit their button when a real font is used. It's made against mpc-vst-plugins `bf1054c`.
