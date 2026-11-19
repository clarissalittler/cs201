/*
 * A defined-behavior companion to floatmunger1.c.
 *
 * Accessing a float through an int pointer violates C's aliasing rules.  Copy
 * the representation into uint32_t instead, manipulate that unsigned value,
 * and copy it back.  The size checks make the binary32 assumption explicit.
 */

#include <ctype.h>
#include <errno.h>
#include <float.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

_Static_assert(sizeof(float) == sizeof(uint32_t),
               "this demonstration requires a 32-bit float");
_Static_assert(FLT_RADIX == 2 && FLT_MANT_DIG == 24 && FLT_MAX_EXP == 128,
               "this demonstration requires IEEE 754 binary32 float");

static uint32_t floatBits(float value) {
  uint32_t bits;
  memcpy(&bits, &value, sizeof(bits));
  return bits;
}

static float bitsFloat(uint32_t bits) {
  float value;
  memcpy(&value, &bits, sizeof(value));
  return value;
}

static void printBits(uint32_t bits) {
  for (int bit = 31; bit >= 0; --bit) {
    putchar((bits & (UINT32_C(1) << (unsigned)bit)) ? '1' : '0');
    if (bit == 31 || bit == 23) {
      putchar(' ');
    }
  }
  putchar('\n');
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

int main(void) {
  float value = 2.0f;

  for (;;) {
    uint32_t bits = floatBits(value);
    printf("The number is %.9g\n", (double)value);
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
    value = bitsFloat(bits);
  }

  return 0;
}
