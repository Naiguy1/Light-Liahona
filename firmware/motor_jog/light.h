#ifndef LIAHONA_LIGHT_H
#define LIAHONA_LIGHT_H
#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif
// Raw inputs are E/N/W/S. Returns 0..359 degrees CCW from East, or -1 if balanced.
int16_t light_angle(const uint16_t raw[4]);
#ifdef __cplusplus
}
#endif
#endif
