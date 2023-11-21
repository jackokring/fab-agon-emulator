#!/usr/bin/bash

export TZ="UTC"
# add commit push
gacp () {
	date=$(date +"%A %Y-%m-%d %H:%M:%S")
  message="${1:-$date}"
  git add . ; git commit -m "$message" ; git push
}

echo "Building VDP 6.66 Rolling Distribution in 'fab-agon-emulator'?"
rm src/vdp/vdp_quark666.so
rm src/vdp/vdp_quark666.o
make
echo "Any Errors? Maybe ^C and try again."
echo "Have you made 'firmware.bin' using VSCode?"
echo "Don't forget to // #define EMULATED from 'agon.h'." 
read -p "Press any key to resume ..."
cp -i src/vdp/vdp-quark666/.pio/build/esp32dev/firmware.bin src/vdp/vdp-quark666/.pio/build/project.checksum sdcard
echo "Have you deleted the // to #define EMULATED again?"
echo "Don't forget any changes to 'STATUS.md' and 'README.md'."
echo "Have you updated the 'mos.bin'? Please manual copy it (if necessary)."
echo "This would also be an ideal time to update 'bbcbasic.bin' and other tools."
echo "Any userspace-vdp-gl updates have to be commited manually."
echo "This is because of backports to 'vdp-gl' might have to be done."
read -p "Press any key to resume ..."
echo "Publishing VDP Source."
pushd src/vdp/vdp-quark666
# git add commit push auto dated commit
gacp
popd
read -p "Press any key to resume ..."
echo "Publishing FOF Source (just in case for SD card changes)."
pushd sdcard/agon-forth/fof
echo "Making 'kernal.bin' using 'gforth'."
gforth crossz80.4th
echo "Compiling glossary of fof words."
gforth doglos.4th
echo "Were there any errors?"
read -p "Press any key to resume ..."
# git add commit push auto dated commit
gacp
popd
echo "Publishing SD Card."
pushd sdcard
# git add commit push auto dated commit
gacp
popd
read -p "Press any key to resume ..."
echo "Publishing Emulator Source."
# git add commit push auto dated commit
gacp

