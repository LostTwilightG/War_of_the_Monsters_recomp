#!/bin/sh
# Download the ee-gcc 2.9x builds that decomp.me hosts into ~/compilers/<name>/
set -e
mkdir -p ~/compilers && cd ~/compilers
for n in "$@"; do
    d=${n%.tar.*}
    [ -d "$d" ] && [ "$(ls -A "$d")" ] && continue
    mkdir -p "$d"
    curl -sfL "https://github.com/decompme/compilers/releases/download/compilers/$n" -o "/tmp/$n"
    tar -xf "/tmp/$n" -C "$d"
done
