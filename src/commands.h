#pragma once

#define NCOMMANDS 9
#define MAX_COMMAND_LENGTH 5

extern const int  COMMAND_ARG_NUMBER[NCOMMANDS];
// argument number of a command

extern const char COMMAND_NAME[NCOMMANDS][MAX_COMMAND_LENGTH];
// name of a command

enum ASM_COMMANDS
{
    ASM_HLT  = 0,
    ASM_PUSH = 1,
    ASM_CUSH = 2,
    ASM_ADD  = 3,
    ASM_SUB  = 4,
    ASM_MUL  = 5,
    ASM_DEL  = 6,
    ASM_DUPE = 7,
    ASM_OUT  = 8,
};
// command codes

