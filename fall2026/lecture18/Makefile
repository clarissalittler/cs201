CC = gcc
# No -g here on purpose: debug info adds a dozen .debug_* sections, and we
# want readelf -S to show just the sections the linker and loader care about.
CFLAGS = -std=gnu17 -Wall -Wextra -O0

PROGRAMS = hello prog prog-lib weak-default weak-override elfpeek \
           usegreet usegreet-rpath dice whereami
LIBS = libhelper.a libgreet.so fakerand.so

# The dup_*.c and mismatch_*.c examples are left out on purpose: the
# link errors are the point, so we type those gcc commands ourselves.
# See LinkerSteps.md.

all: $(PROGRAMS) $(LIBS)

%: %.c
	$(CC) $(CPPFLAGS) $(CFLAGS) $< $(LDFLAGS) $(LDLIBS) -o $@

# Two object files, linked together.
main.o helper.o: helper.h

prog: main.o helper.o
	$(CC) $^ -o $@

# A static library is an archive of object files. The linker copies in only
# the members that define something still undefined, and it reads left to
# right: the library has to come AFTER the file that needs it.
libhelper.a: helper.o
	ar rcs $@ $^

prog-lib: main.o libhelper.a
	$(CC) main.o -L. -lhelper -o $@

# Built from .o files so that `nm weak_main.o` has something to look at.
weak-default: weak_main.o
	$(CC) $^ -o $@

weak-override: weak_main.o weak_override.o
	$(CC) $^ -o $@

# Shared libraries: position-independent code (-fPIC), built with -shared.
libgreet.so: greet.c
	$(CC) $(CFLAGS) -fPIC -shared $< -o $@

fakerand.so: fakerand.c
	$(CC) $(CFLAGS) -fPIC -shared $< -o $@

# Linked against libgreet.so, but with no hint about where to find it.
usegreet: usegreet.c libgreet.so
	$(CC) $(CFLAGS) usegreet.c -L. -lgreet -o $@

# RUNPATH $ORIGIN means "look in the directory this program is in".
usegreet-rpath: usegreet.c libgreet.so
	$(CC) $(CFLAGS) usegreet.c -L. -lgreet -Wl,-rpath,'$$ORIGIN' -o $@

# Everything copied in, nothing loaded at run time. Not part of `all`
# because it needs the static C library (libc.a) installed.
hello-static: hello.c
	$(CC) $(CFLAGS) -static $< -o $@

clean:
	rm -f $(PROGRAMS) $(LIBS) hello-static *.o hello.i hello.s *-patched \
	      prog-stripped dup mismatch dup_b_static.c a.out

.PHONY: all clean
