#!/usr/bin/env bash
set -euo pipefail

VERSION="4.6.0.20260123"
SHA256="6ff27e3bf022666f43f7802255be680eeff722ac181b1725d21e2e8318604ee3"
URL="https://sourceware.org/pub/newlib/newlib-${VERSION}.tar.gz"
TARGET="${OESDK_TARGET:-x86_64-elf}"
ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
WORK="${OESDK_BUILD_ROOT:-$ROOT/Build/Newlib}"
PREFIX="${OESDK_PREFIX:-$ROOT/Output/$TARGET}"
JOBS="${OESDK_JOBS:-$(getconf _NPROCESSORS_ONLN 2>/dev/null || echo 2)}"

command -v "${TARGET}-gcc" >/dev/null || {
  echo "[FAIL] ${TARGET}-gcc was not found. Install or build an ELF cross compiler first." >&2
  exit 1
}
command -v curl >/dev/null || { echo "[FAIL] curl was not found." >&2; exit 1; }
command -v sha256sum >/dev/null || { echo "[FAIL] sha256sum was not found." >&2; exit 1; }

mkdir -p "$WORK" "$PREFIX"
ARCHIVE="$WORK/newlib-${VERSION}.tar.gz"
SOURCE="$WORK/newlib-${VERSION}"
BUILD_DEBUG="$WORK/build-debug"
BUILD_RELEASE="$WORK/build-release"

if [[ ! -f "$ARCHIVE" ]]; then
  curl --fail --location --proto '=https' --tlsv1.2 --retry 3 -o "$ARCHIVE" "$URL"
fi
echo "${SHA256}  ${ARCHIVE}" | sha256sum --check --status || {
  rm -f "$ARCHIVE"
  echo "[FAIL] Newlib source SHA-256 did not match." >&2
  exit 1
}
echo "[ OK ] Verified Newlib ${VERSION}."

if [[ ! -d "$SOURCE" ]]; then
  tar -xzf "$ARCHIVE" -C "$WORK"
fi

build_one() {
  local mode="$1" build="$2" cflags="$3" install="$PREFIX/$mode"
  rm -rf "$build"
  mkdir -p "$build" "$install"
  pushd "$build" >/dev/null
  CFLAGS_FOR_TARGET="$cflags -ffreestanding -fno-stack-protector" \
  "$SOURCE/configure" \
    --target="$TARGET" \
    --prefix="$install" \
    --disable-nls \
    --enable-newlib-reent-small \
    --enable-newlib-retargetable-locking \
    --enable-newlib-multithread \
    --enable-newlib-io-long-long \
    --enable-newlib-io-long-double \
    --enable-newlib-io-c99-formats \
    --enable-newlib-register-fini \
    --enable-newlib-mb \
    --disable-newlib-supplied-syscalls
  make -j"$JOBS" all-target-newlib
  make install-target-newlib
  popd >/dev/null
  echo "[ OK ] Built Newlib $mode for $TARGET."
}

build_one Debug "$BUILD_DEBUG" "-Og -g3"
build_one Release "$BUILD_RELEASE" "-O2 -g"

for mode in Debug Release; do
  install="$PREFIX/$mode/$TARGET"
  mkdir -p "$install/include/oesdk" "$install/lib"
  cp "$ROOT/Packages/LibC/Include/oesdk/libc.h" "$install/include/oesdk/"
  "${TARGET}-gcc" -c "$ROOT/Packages/LibC/Source/OesdkLibcHost.c" \
      -I"$install/include" -ffreestanding -fno-stack-protector \
      $([[ "$mode" == Debug ]] && echo "-Og -g3" || echo "-O2 -g") \
      -o "$WORK/OesdkLibcHost-${mode}.o"
  "${TARGET}-gcc" -c "$ROOT/Packages/LibC/Source/OesdkNewlibSyscalls.c" \
      -I"$install/include" -ffreestanding -fno-stack-protector \
      $([[ "$mode" == Debug ]] && echo "-Og -g3" || echo "-O2 -g") \
      -o "$WORK/OesdkNewlibSyscalls-${mode}.o"
  "${TARGET}-gcc" -c "$ROOT/Packages/LibC/Source/OesdkNewlibLocks.c" \
      -I"$install/include" -ffreestanding -fno-stack-protector \
      $([[ "$mode" == Debug ]] && echo "-Og -g3" || echo "-O2 -g") \
      -o "$WORK/OesdkNewlibLocks-${mode}.o"
  "${TARGET}-ar" rcs "$install/lib/liboesdk-newlib-port.a" \
      "$WORK/OesdkLibcHost-${mode}.o" \
      "$WORK/OesdkNewlibSyscalls-${mode}.o" \
      "$WORK/OesdkNewlibLocks-${mode}.o"
done

echo "[ OK ] OESDK Newlib port installed under $PREFIX."
