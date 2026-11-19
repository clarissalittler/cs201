/*
 * A defined-behavior companion to bitmunger1.c.
 *
 * The original file is intentionally kept as the tiny version used during
 * live lecture.  This version shows the details production C must get right:
 * use an unsigned fixed-width type for bit operations, validate shift counts,
 * and check the quit command before shifting.
 */

#include <ctype.h>
#include <errno.h>
#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

static void printBits(uint32_t value) {
  for (int bit = 31; bit >= 0; --bit) {
    putchar((value & (UINT32_C(1) << (unsigned)bit)) ? '1' : '0');
    if (bit != 0 && bit % 8 == 0) {
      putchar(' ');
    }
  }
  putchar('\n');
}

/* Describe the same 32 bits as a mathematical two's-complement value without
 * relying on an implementation-defined uint32_t-to-int32_t conversion. */
static int64_t signedValue(uint32_t bits) {
  if ((bits & UINT32_C(0x80000000)) == 0) {
    return (int64_t)bits;
  }
  return (int64_t)bits - INT64_C(4294967296);
}

/* Returns 1 for a valid bit, 0 for quit/EOF, and -1 for invalid input. */
static int readBit(unsigned *bit) {
  char line[128];
  printf("Enter a bit to flip (0-31, or q to quit): ");
  if (fgets(line, sizeof(line), stdin) == NULL) {
    return 0;
  }

  char *p = line;
  while (isspace((unsigned char)*p)) {
    ++p;
  }
  if ((*p == 'q' || *p == 'Q') &&
      (p[1] == '\0' || isspace((unsigned char)p[1]))) {
    return 0;
  }

  errno = 0;
  char *end;
  long parsed = strtol(p, &end, 10);
  while (isspace((unsigned char)*end)) {
    ++end;
  }
  if (errno != 0 || end == p || *end != '\0' || parsed < 0 || parsed > 31) {
    return -1;
  }

  *bit = (unsigned)parsed;
  return 1;
}

static int wantsNegation(void) {
  char line[128];
  printf("Apply two's-complement negation? [y/N]: ");
  if (fgets(line, sizeof(line), stdin) == NULL) {
    return 0;
  }
  return line[0] == 'y' || line[0] == 'Y';
}

int main(void) {
  uint32_t bits = UINT32_C(4);

  for (;;) {
    printf("Unsigned: %" PRIu32 "; signed interpretation: %" PRId64 "\n",
           bits, signedValue(bits));
    printBits(bits);

    unsigned bit;
    int status = readBit(&bit);
    if (status == 0) {
      break;
    }
    if (status < 0) {
      fputs("Please enter a bit number from 0 through 31, or q.\n", stderr);
      continue;
    }

    bits ^= UINT32_C(1) << bit;
    if (wantsNegation()) {
      /* Unsigned arithmetic wraps modulo 2^32, so this is fully defined. */
      bits = ~bits + UINT32_C(1);
    }
  }

  return 0;
}
