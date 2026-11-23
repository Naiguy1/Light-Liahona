#include "jog.h"
#include <string.h>

// Half-step sequence in IN1, IN2, IN3, IN4 order.
static const uint8_t phases[8] = {0x1, 0x3, 0x2, 0x6, 0x4, 0xc, 0x8, 0x9};

void jog_init(jog_state *state) {
    memset(state, 0, sizeof(*state));
}

bool jog_start(jog_state *state, int8_t direction, uint32_t now) {
    if (state->active || (direction != 1 && direction != -1)) return false;
    state->started_at = now;
    state->last_step_at = now;
    state->remaining = JOG_STEPS;
    state->direction = direction;
    state->active = true;
    return true;
}

void jog_stop(jog_state *state) {
    state->active = false;
    state->remaining = 0;
    state->outputs = 0;
}

jog_result jog_tick(jog_state *state, uint32_t now) {
    if (!state->active) return JOG_IDLE;
    // Unsigned subtraction also handles millis() wraparound.
    if ((uint32_t)(now - state->started_at) >= JOG_TIMEOUT_MS) {
        jog_stop(state);
        return JOG_TIMED_OUT;
    }
    if ((uint32_t)(now - state->last_step_at) < JOG_INTERVAL_MS) return JOG_RUNNING;
    state->last_step_at = now;
    if (state->remaining == 0) {
        jog_stop(state);
        return JOG_DONE;
    }
    state->phase = (uint8_t)((state->phase + (state->direction > 0 ? 1 : 7)) & 7);
    state->outputs = phases[state->phase];
    --state->remaining;
    return JOG_RUNNING;
}
