#ifndef LIAHONA_JOG_H
#define LIAHONA_JOG_H

#include <stdbool.h>
#include <stdint.h>

#define JOG_STEPS 128U
#define STEPS_PER_REVOLUTION 4096U
#define JOG_INTERVAL_MS 2U
#define JOG_TIMEOUT_MS 1500U
#define MOVE_TIMEOUT_MARGIN_MS 500U

typedef struct {
    uint32_t started_at;
    uint32_t last_step_at;
    uint32_t timeout_ms;
    uint16_t remaining;
    uint16_t position; // Estimated half-steps CCW from manual East zero, modulo 4096.
    uint8_t phase;
    uint8_t outputs; // bits 0..3 correspond to IN1..IN4
    int8_t direction; // +1 is f / CCW; -1 is r / CW, viewed from shaft end.
    bool active;
    bool position_known;
} jog_state;

typedef enum { JOG_IDLE, JOG_RUNNING, JOG_DONE, JOG_TIMED_OUT } jog_result;
typedef enum {
    MOVE_STARTED, MOVE_AT_TARGET, MOVE_BUSY, MOVE_NEEDS_ZERO, MOVE_BAD_ANGLE
} move_result;

#ifdef __cplusplus
extern "C" {
#endif
void jog_init(jog_state *state);
bool jog_start(jog_state *state, int8_t direction, uint32_t now);
bool jog_set_zero(jog_state *state);
move_result jog_move_to(jog_state *state, uint16_t degrees, uint32_t now);
uint16_t jog_angle_tenths(const jog_state *state);
// Explicit stop invalidates zero; ordinary completion preserves its estimate.
void jog_stop(jog_state *state);
jog_result jog_tick(jog_state *state, uint32_t now);
#ifdef __cplusplus
}
#endif
#endif
