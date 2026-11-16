#!/usr/bin/env bash
# reloc.sh: the same function before and after linking.
#
#   ./reloc.sh main.o prog            look at main
#   ./reloc.sh main.o prog main
#
# In the object file, every call to a function in another file is
# e8 00 00 00 00: a call with a zero offset, and a relocation note under it
# telling the linker what to put there. In the linked program, those four
# bytes hold the real distance to the function.

set -euo pipefail

if [[ $# -lt 2 || $# -gt 3 ]]; then
  echo "usage: $0 OBJECT.o PROGRAM [FUNCTION]" >&2
  exit 1
fi
obj=$1
exe=$2
func=${3:-main}

# objdump prints a few header lines first; keep just the function, which
# ends at the next blank line.
disasm() {
  objdump "$@" --disassemble="$func" | sed -n "/<$func>:/,/^\$/p" | sed '/^$/d'
}

echo "==== $func in $obj, BEFORE linking   [objdump -dr --disassemble=$func $obj]"
disasm -dr "$obj"

echo
echo "==== $func in $exe, AFTER linking   [objdump -d --disassemble=$func $exe]"
disasm -d "$exe"

echo
echo "==== Just the calls"
echo "before:"
disasm -dr "$obj" | grep -A1 -E '\scall' | grep -v '^--$' | sed 's/^/  /'
echo "after:"
disasm -d "$exe" | grep -E '\scall' | sed 's/^/  /'
cat <<'EOF'

The 4 bytes after e8 are little-endian and signed. They are the distance
from the NEXT instruction to the target:
    target = (address of the call) + 5 + offset
Try it on one of the "after" lines and see if you land on the function.
EOF
