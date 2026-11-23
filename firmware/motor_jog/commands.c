#include "commands.h"
#include <string.h>

void command_init(command_parser *parser) {
    memset(parser, 0, sizeof(*parser));
}

motor_command command_feed(command_parser *parser, char input) {
    motor_command command = {COMMAND_NONE, 0};
    if (input == 's') {
        command_init(parser);
        parser->discard = true;
        command.kind = COMMAND_STOP;
        return command;
    }
    if (input != '\r' && input != '\n') {
        if (parser->discard) return command;
        if (parser->length >= sizeof(parser->text)) parser->overflow = true;
        else parser->text[parser->length++] = input;
        return command;
    }
    if (parser->discard || (!parser->length && !parser->overflow)) {
        command_init(parser);
        return command;
    }
    command.kind = COMMAND_INVALID;
    if (!parser->overflow) {
        if (parser->length == 1) {
            switch (parser->text[0]) {
                case 'f': command.kind = COMMAND_FORWARD; break;
                case 'r': command.kind = COMMAND_REVERSE; break;
                case 'z': command.kind = COMMAND_ZERO; break;
                case 'p': command.kind = COMMAND_POSITION; break;
                case 'l': command.kind = COMMAND_LIGHT; break;
                case 't': command.kind = COMMAND_TRACK; break;
                case 'c': command.kind = COMMAND_CALIBRATE; break;
                case '?': command.kind = COMMAND_HELP; break;
            }
        }
        if (command.kind == COMMAND_INVALID) {
            bool numeric = true;
            for (uint8_t i = 0; i < parser->length; ++i) {
                char digit = parser->text[i];
                if (digit < '0' || digit > '9') { numeric = false; break; }
                command.degrees = command.degrees * 10U + (uint16_t)(digit - '0');
            }
            if (numeric && command.degrees <= 360U) command.kind = COMMAND_ANGLE;
        }
    }
    command_init(parser);
    return command;
}
