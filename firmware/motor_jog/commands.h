#ifndef LIAHONA_COMMANDS_H
#define LIAHONA_COMMANDS_H
#include <stdbool.h>
#include <stdint.h>

typedef enum {
    COMMAND_NONE, COMMAND_FORWARD, COMMAND_REVERSE, COMMAND_STOP,
    COMMAND_ZERO, COMMAND_POSITION, COMMAND_ANGLE, COMMAND_LIGHT, COMMAND_TRACK, COMMAND_CALIBRATE, COMMAND_HELP, COMMAND_INVALID
} command_kind;
typedef struct { command_kind kind; uint16_t degrees; } motor_command;
typedef struct {
    char text[3];
    uint8_t length;
    bool overflow;
    bool discard;
} command_parser;
#ifdef __cplusplus
extern "C" {
#endif
void command_init(command_parser *parser);
// All commands end with CR/LF, except lowercase s which stops immediately.
// After s, the rest of that line is discarded.
motor_command command_feed(command_parser *parser, char input);
#ifdef __cplusplus
}
#endif
#endif
