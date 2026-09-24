#include <inttypes.h> /* fixed-width integer formats */
#include <stdint.h>   /* uint32_t, int64_t */
#include <stdio.h>    /* printf */

int main(void) {
    uint32_t flags = UINT32_C(0x05);
    const uint32_t write_permission = UINT32_C(1) << 1;
    printf("Initial flags: 0x%02" PRIx32 "\n", flags);
    flags |= write_permission;
    printf("Set bit 1:     0x%02" PRIx32 "\n", flags);
    flags &= ~write_permission;
    printf("Clear bit 1:   0x%02" PRIx32 "\n", flags);

    /* Interpret an eight-bit pattern by arithmetic, without an
       out-of-range conversion to a signed C type. */
    const uint32_t pattern = UINT32_C(0xfd);
    const int64_t signed_value = (int64_t)pattern - INT64_C(256);
    printf("Eight bits fd: unsigned %" PRIu32 ", signed %" PRId64 "\n",
           pattern, signed_value);
    printf("-1 converted to uint32_t: %" PRIu32 "\n", (uint32_t)-1);
    printf("UINT32_MAX + 1: %" PRIu32 "\n", UINT32_MAX + UINT32_C(1));
    /* Do not turn the last expression into INT32_MAX + 1:
       signed overflow is undefined behavior, not a wraparound rule. */
    return 0;
}
