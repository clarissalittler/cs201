#!/usr/bin/env bash
# peek.sh: a quick tour of an ELF file using the standard tools.
#
#   ./peek.sh hello          a program
#   ./peek.sh main.o         an object file
#   ./peek.sh libgreet.so    a shared library
#
# Each section below prints the command it used, so you can run that one
# on its own and read the full output.

set -uo pipefail

if [[ $# -ne 1 ]]; then
  echo "usage: $0 ELF-FILE" >&2
  exit 1
fi
f=$1
if [[ ! -f $f ]]; then
  echo "$0: $f: no such file" >&2
  exit 1
fi
if [[ $(head -c 4 "$f" | od -An -tx1 | tr -d ' ') != 7f454c46 ]]; then
  echo "$0: $f is not an ELF file (it doesn't start with \\x7fELF)" >&2
  exit 1
fi

banner() {
  echo
  echo "==== $1   [$2]"
}

type=$(readelf -h "$f" | awk '/Type:/ { print $2 }')

banner "What is it?" "file $f"
file "$f"

banner "ELF header" "readelf -h $f"
readelf -hW "$f" | grep -E 'Class|Type:|Machine|Entry point|Number of (program|section) headers'
case $type in
  REL)  echo "  REL: an object file. It has holes for the linker to fill." ;;
  EXEC) echo "  EXEC: a program that must be loaded at a fixed address." ;;
  DYN)  echo "  DYN: a shared library, or a program that can load anywhere (PIE)." ;;
esac

banner "Sections: what the linker works with" "readelf -S $f"
readelf -SW "$f" | sed -n '/^ *\[Nr\]/,/^Key to Flags/p' | grep -v '^Key to Flags'
echo "  Flags: A = loaded into memory, W = writable, X = executable."

if [[ $type != REL ]]; then
  banner "Segments: what the loader maps, with permissions" "readelf -l $f"
  readelf -lW "$f" | sed -n '/^ *Type /,/^$/p'
  echo "Which sections go in which segment:"
  readelf -lW "$f" | sed -n '/Section to Segment mapping/,$p' | tail -n +3
fi

banner "Global symbols defined here" "nm --defined-only -g $f"
defined=$(nm --defined-only -g "$f" 2>/dev/null)
if [[ -z $defined ]]; then
  echo "  (none: was it stripped? try nm -D for the dynamic symbols)"
else
  echo "$defined" | head -n 20
  n=$(echo "$defined" | wc -l)
  (( n > 20 )) && echo "  ... and $((n - 20)) more"
fi

banner "Symbols it needs from somewhere else" "nm -u $f"
needs=$(nm -u "$f" 2>/dev/null)
if [[ -z $needs ]]; then
  needs=$(nm -D -u "$f" 2>/dev/null)
fi
echo "${needs:-  (none)}"

if [[ $type == REL ]]; then
  banner "Relocations: the holes and what goes in them" "readelf -r $f"
  readelf -rW "$f" | grep -vE '^$'
else
  banner "Shared libraries it needs at run time" "readelf -d $f"
  libs=$(readelf -dW "$f" | grep -E 'NEEDED|RUNPATH|RPATH|SONAME')
  echo "${libs:-  (none: a static program, or no dynamic section)}"
fi
