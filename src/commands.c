#include "commands.h"

const char COMMAND_NAME[NCOMMANDS][MAX_COMMAND_LENGTH] = {
    "HLT",
    "PUSH",
    "CUSH",
    "ADD",
    "SUB",
    "MUL",
    "DEL",
    "DUPE",
    "OUT"
};

const int COMMAND_ARG_NUMBER[NCOMMANDS] = {
    0,
    1,
    0,
    0,
    0,
    0,
    0,
    0
};
