#!/usr/bin/env bash
# pipeline.sh: run each stage of a C build by hand and keep every file.
#
#   ./pipeline.sh hello.c
#
# Normally `gcc hello.c -o hello` does all four steps and throws away the
# files in between. Here we do them one at a time:
#
#   hello.c --preprocess--> hello.i --compile--> hello.s
#           --assemble----> hello.o --link-----> hello

set -euo pipefail

if [[ $# -ne 1 || $1 != *.c ]]; then
  echo "usage: $0 FILE.c" >&2
  exit 1
fi
src=$1
base=${src%.c}

# Print a command, then run it.
run() {
  echo "\$ $*"
  "$@"
}

banner() {
  echo
  echo "==== $* ===="
}

banner "1. Preprocess: paste in #includes, expand #defines"
run gcc -E "$src" -o "$base.i"
echo "$base.c is $(wc -l < "$src") lines; $base.i is $(wc -l < "$base.i") lines."
echo "Our own code is at the very end, with the macros already replaced:"
tail -n 5 "$base.i"

banner "2. Compile: C to assembly"
run gcc -S "$base.i" -o "$base.s"
echo "Look for the calls. Each target is just a name so far."
echo "(Is it the function you actually wrote in $src?)"
grep -n 'call' "$base.s" || echo "(no calls in this file)"

banner "3. Assemble: assembly to an object file"
run gcc -c "$base.s" -o "$base.o"
file "$base.o"
echo "Symbols (U = used here but defined somewhere else):"
nm "$base.o"

banner "4. Link: object files to a program"
run gcc "$base.o" -o "$base"
file "$base"

banner "What did gcc actually pass to the linker?"
# -### prints the commands gcc would run, without running them. One of them
# is collect2, gcc's wrapper around ld. Pull out the files it links together.
gcc -### "$base.o" -o "$base" 2>&1 | grep -m 1 collect2 | tr ' ' '\n' | tr -d '"' |
  grep -E '\.o$|^-l' | sed 's|^.*/||; s/^/  /'
echo "We only wrote $base.o. The crt*.o files are the C runtime: crt1.o"
echo "(Scrt1.o for a PIE) holds _start, which sets things up and calls main."
echo "-lc is the C library, where printf and puts live."
nm "$base" | grep -w -e _start -e main

banner "Files left behind"
ls -l "$base.i" "$base.s" "$base.o" "$base"
