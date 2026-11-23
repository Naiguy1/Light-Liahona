#include <assert.h>
#include <stdio.h>
#include "jog.h"

int main(void) {
    jog_state s;
    jog_init(&s);
    assert(!s.active && !s.outputs);
    assert(!jog_start(&s, 0, 0));
    assert(jog_start(&s, 1, 100));
    assert(!jog_start(&s, -1, 101));
    assert(jog_tick(&s, 104) == JOG_RUNNING && s.outputs == 0);
    assert(jog_tick(&s, 105) == JOG_RUNNING && s.outputs == 0x3);
    for (uint32_t i = 2; i <= JOG_STEPS; ++i) {
        assert(jog_tick(&s, 100 + i * JOG_INTERVAL_MS) == JOG_RUNNING);
        assert(s.outputs != 0 && s.outputs <= 0xf);
    }
    assert(s.remaining == 0 && s.active);
    assert(jog_tick(&s, 100 + (JOG_STEPS + 1) * JOG_INTERVAL_MS) == JOG_DONE);
    assert(!s.outputs && !s.active);
    assert(jog_start(&s, -1, 1000));
    assert(jog_tick(&s, 1005) == JOG_RUNNING && s.outputs == 0x9);
    jog_stop(&s);
    assert(!s.outputs && !s.active);
    assert(jog_tick(&s, 1010) == JOG_IDLE && !s.outputs);
    assert(jog_start(&s, 1, 2000));
    assert(jog_tick(&s, 2005) == JOG_RUNNING && s.outputs != 0);
    assert(jog_tick(&s, 2000 + JOG_TIMEOUT_MS) == JOG_TIMED_OUT);
    assert(!s.outputs && !s.active);
    jog_init(&s);
    assert(jog_start(&s, 1, UINT32_MAX - 2));
    assert(jog_tick(&s, 2) == JOG_RUNNING && s.outputs == 0x3);
    puts("jog tests passed: bounded motion, stop, timeout, reverse and clock wrap");
    return 0;
}
