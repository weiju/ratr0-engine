#include "utils.h"

#ifdef DEBUG
#include <stdio.h>
extern FILE *debug_fp;
#endif

UINT8 num_digits(UINT16 value)
{
    UINT8 result = 1;
    UINT16 tmp = value / 10;
    while (tmp > 0) {
        tmp /= 10;
        result++;
    }
    return result;
}

UINT8 extract_digits(UINT8 *digit_buffer, UINT8 max_digits, UINT16 value)
{
    UINT8 index = 0;
    UINT8 out_digit = value % 10;
    UINT16 cur_value = value / 10;
    digit_buffer[index++] = '0' + out_digit;
    while (cur_value > 0) {
        out_digit = cur_value % 10;
        digit_buffer[index++] = '0' + out_digit;
        cur_value /= 10;
    }
    return index;
}
