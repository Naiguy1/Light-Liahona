#ifndef LIAHONA_LIGHT_H
#define LIAHONA_LIGHT_H
#include <stdbool.h>
#include <stdint.h>
typedef struct { uint16_t covered[4]; uint16_t bright[4]; } light_calibration;
#ifdef __cplusplus
extern "C" {
#endif
extern const light_calibration LIGHT_DEFAULT_CALIBRATION;
bool light_calibration_valid(const light_calibration *calibration);
uint16_t light_calibration_checksum(const light_calibration *calibration);
// E/N/W/S inputs; -1 means weak or balanced directional differences.
int16_t light_angle_calibrated(const uint16_t raw[4], const light_calibration *calibration);
int16_t light_angle(const uint16_t raw[4]);
#ifdef __cplusplus
}
#endif
#endif
