#ifndef LIAHONA_JOG_H
#define LIAHONA_JOG_H

#include <stdbool.h>
#include <stdint.h>

#define JOG_STEPS 128U
#define JOG_INTERVAL_MS 5U
#define JOG_TIMEOUT_MS 1500U

typedef struct {
    uint32_t started_at;
    uint32_t last_step_at;
    uint16_t remaining;
    uint8_t phase;
    uint8_t outputs; // bits 0..3 correspond to IN1..IN4
    int8_t direction;
    bool active;
} jog_state;

typedef enum { JOG_IDLE, JOG_RUNNING, JOG_DONE, JOG_TIMED_OUT } jog_result;

#ifdef __cplusplus
extern "C" {
#endif
void jog_init(jog_state *state);
bool jog_start(jog_state *state, int8_t direction, uint32_t now);
void jog_stop(jog_state *state);
jog_result jog_tick(jog_state *state, uint32_t now);
#ifdef __cplusplus
}
#endif
#endif
