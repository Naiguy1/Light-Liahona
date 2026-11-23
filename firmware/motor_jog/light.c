#include "light.h"
#include <math.h>

const light_calibration LIGHT_DEFAULT_CALIBRATION = {
    {165, 155, 165, 75}, {967, 956, 971, 932}
};

bool light_calibration_valid(const light_calibration *calibration) {
    for (uint8_t i = 0; i < 4; ++i) {
        if (calibration->bright[i] > 1023 || calibration->covered[i] > 1023 ||
            calibration->bright[i] <= calibration->covered[i] ||
            calibration->bright[i] - calibration->covered[i] < 100) return false;
    }
    return true;
}

uint16_t light_calibration_checksum(const light_calibration *calibration) {
    uint16_t crc = 0xffff;
    for (uint8_t i = 0; i < 8; ++i) {
        uint16_t value = i < 4 ? calibration->covered[i] : calibration->bright[i - 4];
        for (uint8_t byte = 0; byte < 2; ++byte) {
            crc ^= value & 0xff;
            value >>= 8;
            for (uint8_t bit = 0; bit < 8; ++bit)
                crc = (crc & 1) ? (crc >> 1) ^ 0xa001 : crc >> 1;
        }
    }
    return crc;
}

int16_t light_angle_calibrated(const uint16_t raw[4], const light_calibration *calibration) {
    if (!light_calibration_valid(calibration)) return -1;
    int16_t normalized[4];
    for (uint8_t i = 0; i < 4; ++i) {
        int32_t value = ((int32_t)raw[i] - calibration->covered[i]) * 1000L /
                        (calibration->bright[i] - calibration->covered[i]);
        if (value < 0) value = 0;
        if (value > 1000) value = 1000;
        normalized[i] = (int16_t)value;
    }
    const int16_t x = normalized[0] - normalized[2];
    const int16_t y = normalized[1] - normalized[3];
    // Ignore directional contrast below 10% of the normalized sensor range.
    if ((int32_t)x * x + (int32_t)y * y < 10000L) return -1;
    float degrees = atan2f((float)y, (float)x) * 57.2957795f;
    if (degrees < 0) degrees += 360.0f;
    return (int16_t)((uint16_t)(degrees + 0.5f) % 360U);
}

int16_t light_angle(const uint16_t raw[4]) {
    return light_angle_calibrated(raw, &LIGHT_DEFAULT_CALIBRATION);
}
