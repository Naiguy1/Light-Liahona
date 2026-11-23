#include "jog.h"
#include <string.h>

static const uint8_t phases[8] = {0x1, 0x3, 0x2, 0x6, 0x4, 0xc, 0x8, 0x9};

static void release(jog_state *state) {
    state->active = false;
    state->remaining = 0;
    state->outputs = 0;
}

static void start_steps(jog_state *state, int8_t direction, uint16_t steps,
                        uint32_t now) {
    state->started_at = now;
    state->last_step_at = now;
    // Longer absolute moves need more time than the original 128-step jog.
    state->timeout_ms = ((uint32_t)steps + 1U) * JOG_INTERVAL_MS + MOVE_TIMEOUT_MARGIN_MS;
    if (state->timeout_ms < JOG_TIMEOUT_MS) state->timeout_ms = JOG_TIMEOUT_MS;
    state->remaining = steps;
    state->direction = direction;
    state->active = true;
}

void jog_init(jog_state *state) {
    memset(state, 0, sizeof(*state));
}

bool jog_start(jog_state *state, int8_t direction, uint32_t now) {
    if (state->active || (direction != 1 && direction != -1)) return false;
    start_steps(state, direction, JOG_STEPS, now);
    return true;
}

bool jog_set_zero(jog_state *state) {
    if (state->active) return false;
    state->position = 0;
    state->position_known = true;
    return true;
}

move_result jog_move_to(jog_state *state, uint16_t degrees, uint32_t now) {
    if (state->active) return MOVE_BUSY;
    if (degrees > 360U) return MOVE_BAD_ANGLE;
    if (!state->position_known) return MOVE_NEEDS_ZERO;
    // Widen before multiplying: AVR int is 16 bits. Round to nearest half-step.
    uint16_t target = (uint16_t)(((uint32_t)degrees * STEPS_PER_REVOLUTION + 180U) / 360U);
    if (target == STEPS_PER_REVOLUTION) target = 0; // 360 means East, not another lap.
    uint16_t ccw = (uint16_t)((target + STEPS_PER_REVOLUTION - state->position) % STEPS_PER_REVOLUTION);
    if (ccw == 0) return MOVE_AT_TARGET;
    if (ccw <= STEPS_PER_REVOLUTION / 2U) {
        start_steps(state, 1, ccw, now); // Exactly 180 degrees chooses CCW.
    } else {
        start_steps(state, -1, STEPS_PER_REVOLUTION - ccw, now);
    }
    return MOVE_STARTED;
}

uint16_t jog_angle_tenths(const jog_state *state) {
    return (uint16_t)(((uint32_t)state->position * 3600UL + STEPS_PER_REVOLUTION / 2U) /
                      STEPS_PER_REVOLUTION);
}

void jog_stop(jog_state *state) {
    release(state);
    state->position_known = false;
}

jog_result jog_tick(jog_state *state, uint32_t now) {
    if (!state->active) return JOG_IDLE;
    if ((uint32_t)(now - state->started_at) >= state->timeout_ms) {
        jog_stop(state);
        return JOG_TIMED_OUT;
    }
    if ((uint32_t)(now - state->last_step_at) < JOG_INTERVAL_MS) return JOG_RUNNING;
    state->last_step_at = now;
    if (state->remaining == 0) {
        release(state);
        return JOG_DONE;
    }
    state->phase = (uint8_t)((state->phase + (state->direction > 0 ? 1 : 7)) & 7);
    state->outputs = phases[state->phase];
    if (state->direction > 0) {
        state->position = (state->position + 1U) % STEPS_PER_REVOLUTION;
    } else {
        state->position = (state->position + STEPS_PER_REVOLUTION - 1U) % STEPS_PER_REVOLUTION;
    }
    --state->remaining;
    return JOG_RUNNING;
}
