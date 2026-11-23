#include <assert.h>
#include <stdio.h>
#include "light.h"

int main(void) {
    const uint16_t cases[][4] = {
        {967, 155, 165, 75}, // E
        {165, 956, 165, 75}, // N
        {165, 155, 971, 75}, // W
        {165, 155, 165, 932}, // S
        {967, 956, 165, 75}, // NE
        {165, 956, 971, 75}, // NW
        {165, 155, 971, 932}, // SW
        {967, 155, 165, 932}, // SE
        {165, 155, 165, 75}, // Covered endpoints
        {967, 956, 971, 932}, // Bright endpoints
        {0, 0, 0, 0}, // Below covered: clamp before differencing
        {1023, 1023, 1023, 1023}, // Above bright: clamp
        {1023, 0, 0, 0}
    };
    const int16_t expected[] = {0, 90, 180, 270, 45, 135, 225, 315, -1, -1, -1, -1, 0};
    for (unsigned i = 0; i < sizeof(expected) / sizeof(expected[0]); ++i) {
        assert(light_angle(cases[i]) == expected[i]);
    }
    puts("light tests passed: cardinal/diagonal angles, balanced inputs and clamping");
    return 0;
}
