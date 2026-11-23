#include <assert.h>
#include <stdio.h>
#include "jog.h"
#include "commands.h"

static uint32_t now;

static void complete(jog_state *s) {
    uint16_t count = s->remaining;
    uint16_t updates = 0;
    while (s->active) {
        uint16_t previous = s->remaining;
        now += JOG_INTERVAL_MS;
        jog_result result = jog_tick(s, now);
        assert(result == JOG_RUNNING || result == JOG_DONE);
        if (s->remaining < previous) ++updates;
        assert(s->position < STEPS_PER_REVOLUTION);
        assert(updates <= count);
    }
    assert(updates == count && s->outputs == 0);
}

static motor_command parse(command_parser *p, const char *text) {
    motor_command command = {COMMAND_NONE, 0};
    for (; *text; ++text) command = command_feed(p, *text);
    return command;
}

static void positions(void) {
    jog_state s;
    jog_init(&s);
    assert(!s.active && !s.outputs && !s.position_known);
    assert(jog_move_to(&s, 90, now) == MOVE_NEEDS_ZERO);
    assert(jog_move_to(&s, 361, now) == MOVE_BAD_ANGLE);
    assert(jog_move_to(&s, UINT16_MAX, now) == MOVE_BAD_ANGLE);
    assert(!s.active && !s.outputs);
    assert(jog_set_zero(&s));
    assert(s.position_known && !s.outputs);
    assert(jog_move_to(&s, 0, now) == MOVE_AT_TARGET);
    assert(jog_move_to(&s, 360, now) == MOVE_AT_TARGET);

    // Quarter-turn sequence: no forced full revolution when returning to East.
    for (uint16_t angle = 90; angle <= 360; angle += 90) {
        assert(jog_move_to(&s, angle, now) == MOVE_STARTED);
        assert(s.direction == 1 && s.remaining == 1024);
        assert(!jog_set_zero(&s));
        assert(jog_move_to(&s, 0, now) == MOVE_BUSY);
        complete(&s);
        assert(s.position_known);
        assert(jog_angle_tenths(&s) == (angle % 360U) * 10U);
    }
    assert(s.position == 0);
    assert(jog_move_to(&s, 270, now) == MOVE_STARTED);
    assert(s.direction == -1 && s.remaining == 1024);
    complete(&s);
    assert(s.position == 3072);
    assert(jog_move_to(&s, 0, now) == MOVE_STARTED && s.direction == 1);
    complete(&s);

    // All supported angles round to the nearest half-step and use <= half a turn.
    for (uint16_t angle = 0; angle <= 360; ++angle) {
        assert(jog_set_zero(&s));
        uint16_t target = (uint16_t)(((uint32_t)angle * STEPS_PER_REVOLUTION + 180U) / 360U);
        target %= STEPS_PER_REVOLUTION;
        move_result result = jog_move_to(&s, angle, now);
        if (target == 0) assert(result == MOVE_AT_TARGET && !s.active);
        else {
            assert(result == MOVE_STARTED && s.remaining <= 2048);
            assert(s.direction == (angle <= 180 ? 1 : -1));
            complete(&s);
        }
        assert(s.position == target && s.position_known);
    }
    // Shortest path across the zero boundary in both directions.
    assert(jog_move_to(&s, 359, now) == MOVE_STARTED);
    complete(&s);
    assert(jog_move_to(&s, 1, now) == MOVE_STARTED);
    assert(s.direction == 1 && s.remaining == 22);
    complete(&s);
    assert(jog_move_to(&s, 359, now) == MOVE_STARTED);
    assert(s.direction == -1 && s.remaining == 22);
    complete(&s);

    // Jogging after zero updates the estimate; a full measured turn wraps to zero.
    assert(jog_set_zero(&s));
    for (uint8_t jog = 0; jog < 32; ++jog) {
        assert(jog_start(&s, 1, now));
        complete(&s);
    }
    assert(s.position == 0 && s.position_known);
    assert(jog_start(&s, -1, now));
    complete(&s);
    assert(s.position == 3968 && s.position_known);

    // Stop and elapsed-time fault release and require another manual reference.
    assert(jog_move_to(&s, 180, now) == MOVE_STARTED);
    now += JOG_INTERVAL_MS;
    assert(jog_tick(&s, now) == JOG_RUNNING && s.outputs);
    jog_stop(&s);
    assert(!s.active && !s.outputs && !s.position_known);
    assert(jog_move_to(&s, 0, now) == MOVE_NEEDS_ZERO);
    assert(jog_tick(&s, now + 1) == JOG_IDLE && !s.outputs);
    assert(jog_set_zero(&s));
    assert(jog_move_to(&s, 180, now) == MOVE_STARTED);
    assert(s.direction == 1 && s.remaining == 2048);
    assert(s.timeout_ms > 2049UL * JOG_INTERVAL_MS);
    now += s.timeout_ms;
    assert(jog_tick(&s, now) == JOG_TIMED_OUT);
    assert(!s.active && !s.outputs && !s.position_known);

    // A long move can cross the 32-bit millis() boundary without a false timeout.
    assert(jog_set_zero(&s));
    now = UINT32_MAX - 2;
    assert(jog_move_to(&s, 180, now) == MOVE_STARTED);
    complete(&s);
    assert(s.position == 2048 && s.position_known);
    jog_init(&s); // Reset never restores a stale zero.
    assert(!s.position_known && !s.active && !s.outputs);
}

static void commands(void) {
    command_parser p;
    command_init(&p);
    assert(command_feed(&p, '9').kind == COMMAND_NONE);
    assert(command_feed(&p, '0').kind == COMMAND_NONE);
    motor_command c = command_feed(&p, '\n');
    assert(c.kind == COMMAND_ANGLE && c.degrees == 90);
    assert(command_feed(&p, '\n').kind == COMMAND_NONE);
    assert(parse(&p, "f\r").kind == COMMAND_FORWARD);
    assert(command_feed(&p, '\n').kind == COMMAND_NONE); // CRLF doesn't repeat f.
    assert(parse(&p, "r\n").kind == COMMAND_REVERSE);
    assert(parse(&p, "z\n").kind == COMMAND_ZERO);
    assert(parse(&p, "p\n").kind == COMMAND_POSITION);
    assert(parse(&p, "?\n").kind == COMMAND_HELP);
    c = parse(&p, "360\n");
    assert(c.kind == COMMAND_ANGLE && c.degrees == 360);
    const char *invalid[] = {"361\n", "999\n", "-90\n", "90.5\n", "90x\n", "12f\n", "1234567890\n", "f90\n", "90 0\n"};
    for (uint8_t i = 0; i < sizeof(invalid) / sizeof(invalid[0]); ++i) {
        assert(parse(&p, invalid[i]).kind == COMMAND_INVALID);
        assert(parse(&p, "z\n").kind == COMMAND_ZERO); // recovery after bad input
    }
    parse(&p, "18");
    assert(command_feed(&p, 's').kind == COMMAND_STOP); // No newline needed.
    assert(parse(&p, "90\n").kind == COMMAND_NONE); // Stop discards suffix.
    c = parse(&p, "90\n");
    assert(c.kind == COMMAND_ANGLE && c.degrees == 90);
}

int main(void) {
    positions();
    commands();
    puts("position tests passed: all angles, shortest paths, zero, stop, timeout, wrap and parser");
    return 0;
}
