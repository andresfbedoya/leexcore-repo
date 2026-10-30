#!/usr/bin/env bash
set -euo pipefail
NDK="${ANDROID_NDK_HOME:-$ANDROID_HOME/ndk/27.0.12077973}"
TC="$NDK/toolchains/llvm/prebuilt/linux-x86_64/bin"
OUT="$(cd "$(dirname "$0")/.." && pwd)/bin"
SRC="$(cd "$(dirname "$0")" && pwd)/secondhome.c"
mkdir -p "$OUT"
for spec in "arm64-v8a:aarch64-linux-android21-clang" "armeabi-v7a:armv7a-linux-androideabi21-clang" "x86_64:x86_64-linux-android21-clang" "x86:i686-linux-android21-clang"; do
  abi="${spec%%:*}"; cc="${spec#*:}"; mkdir -p "$OUT/$abi"
  "$TC/$cc" -O2 -fPIE -pie -Wall -Wextra "$SRC" -o "$OUT/$abi/secondhome"
  chmod 755 "$OUT/$abi/secondhome"
done
