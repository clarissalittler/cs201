#!/usr/bin/env bash
# patch-string.sh: change a string inside a compiled program.
#
#   ./patch-string.sh hello "pipeline" "linker!!"
#   ./hello-patched
#
# A string literal is just bytes in the file (in .rodata). If we find those
# bytes and overwrite them, the program prints something new. We never touch
# the original: the change goes into a copy called PROGRAM-patched.
#
# The new string can't be longer than the old one. Nothing would move to make
# room, so we'd overwrite whatever comes next. A shorter string gets padded
# with zero bytes, which ends a C string early.

set -euo pipefail
export LC_ALL=C          # count bytes, not characters

if [[ $# -ne 3 ]]; then
  echo "usage: $0 PROGRAM OLD NEW" >&2
  exit 1
fi
bin=$1
old=$2
new=$3
out="$bin-patched"

if [[ -z $old ]]; then
  echo "$0: OLD can't be empty" >&2
  exit 1
fi
if (( ${#new} > ${#old} )); then
  echo "$0: \"$new\" is ${#new} bytes but \"$old\" is only ${#old}." >&2
  echo "The new string has to fit in the old one's space." >&2
  exit 1
fi

# grep -o -b prints BYTEOFFSET:MATCH for every match; -a treats the binary as text.
offsets=$(grep -obaF -- "$old" "$bin" | cut -d: -f1 || true)
count=$(printf '%s' "$offsets" | grep -c . || true)
if (( count != 1 )); then
  echo "$0: found $count copies of \"$old\" in $bin; need exactly 1." >&2
  [[ -n $offsets ]] && echo "Offsets: $(echo $offsets)" >&2
  exit 1
fi
off=$offsets

# Which section is that offset in? readelf -S gives each section's file
# offset and size (in hex). Strip the "[Nr]" column so the fields line up.
section="(no section)"
while read -r name stype addr soff size _; do
  [[ $soff =~ ^[0-9a-f]+$ && $size =~ ^[0-9a-f]+$ ]] || continue
  if (( off >= 16#$soff && off < 16#$soff + 16#$size )); then
    section="$name ($stype, file offset 0x$soff)"
    break
  fi
done < <(readelf -SW "$bin" | grep '^ *\[ *[0-9]' | sed 's/^ *\[ *[0-9]*\] *//')

printf 'Found "%s" at file offset %d (0x%x), in section %s\n' "$old" "$off" "$off" "$section"

cp -- "$bin" "$out"
printf '%s' "$new" | dd of="$out" bs=1 seek="$off" conv=notrunc status=none
pad=$(( ${#old} - ${#new} ))
if (( pad > 0 )); then
  head -c "$pad" /dev/zero | dd of="$out" bs=1 seek=$(( off + ${#new} )) conv=notrunc status=none
fi

echo
echo "before ($bin):"
xxd -s "$off" -l "${#old}" "$bin"
echo "after ($out):"
xxd -s "$off" -l "${#old}" "$out"
echo
echo "Now try:  ./$bin   and   ./$out"
