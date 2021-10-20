#pragma once

#define NCOMMANDS 9
#define MAX_COMMAND_LENGTH 5

extern const int  COMMAND_ARG_NUMBER[NCOMMANDS];
// argument number of a command

extern const char COMMAND_NAME[NCOMMANDS][MAX_COMMAND_LENGTH];
// name of a command

extern const int COMMAND_CODE[NCOMMANDS];
// code of a command

enum ARG_MASKS
{
    ARG_CMD = 0x1F,
    ARG_IMM = 0x20,
    ARG_REG = 0x40,
    ARG_MEM = 0x80,
    ARG_ALL = ARG_IMM | ARG_REG | ARG_MEM
};
// masks of command data

enum ASM_COMMANDS
{
    ASM_HLT  = 0,
    ASM_PUSH = 1,
    ASM_POP  = 2,
    ASM_ADD  = 3,
    ASM_SUB  = 4,
    ASM_MUL  = 5,
    ASM_DEL  = 6,
    ASM_DUPE = 7,
    ASM_OUT  = 8,
};
// command codes

