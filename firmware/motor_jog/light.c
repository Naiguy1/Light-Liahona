#include "light.h"
#include <math.h>

int16_t light_angle(const uint16_t raw[4]) {
    // User-verified covered/bright endpoints, in E/N/W/S order.
    static const int16_t covered[4] = {165, 155, 165, 75};
    static const int16_t bright[4] = {967, 956, 971, 932};
    int16_t normalized[4];
    for (uint8_t i = 0; i < 4; ++i) {
        int32_t value = ((int32_t)raw[i] - covered[i]) * 1000L /
                        (bright[i] - covered[i]);
        if (value < 0) value = 0;
        if (value > 1000) value = 1000;
        normalized[i] = (int16_t)value;
    }
    const int16_t x = normalized[0] - normalized[2]; // East - West
    const int16_t y = normalized[1] - normalized[3]; // North - South
    if (x == 0 && y == 0) return -1;
    float degrees = atan2f((float)y, (float)x) * 57.2957795f;
    if (degrees < 0) degrees += 360.0f;
    return (int16_t)((uint16_t)(degrees + 0.5f) % 360U);
}
