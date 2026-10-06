#!/bin/sh
# Downloads the SN ProDG ee-gcc 2.95.2 build (identified as the retail compiler) into tools/cc/.
set -e
dir=$(cd "$(dirname "$0")" && pwd)/cc
name=ee-gcc2.95.2-SN-v2.73a
[ -x "$dir/$name/cc/bin/ee-gcc.exe" ] && { echo "$name already installed"; exit 0; }
mkdir -p "$dir/$name"
curl -sfL "https://github.com/TheOnlyZac/compilers/releases/download/$name/$name.tar.gz" | tar -xz -C "$dir/$name"
echo "installed $name into $dir/$name"
