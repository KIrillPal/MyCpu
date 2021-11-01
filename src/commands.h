#pragma once

#define NCOMMANDS 19
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
	ASM_IN   = 3,

	ASM_ADD  = 4,
	ASM_SUB  = 5,
	ASM_MUL  = 6,
	ASM_DEL  = 7,
	ASM_DUPE = 8,
	ASM_OUT  = 9,

	ASM_JMP  = 10,
	ASM_JE   = 11,
	ASM_JNE  = 12,
	ASM_JL   = 13,
	ASM_JLE  = 14,
	ASM_JG   = 15,
	ASM_JGE  = 16,
	ASM_CALL = 17,
	ASM_RET  = 18
};
// command codes

enum ASM_ARGTYPE
{
	ARG_NOARG = 0,
	ARG_VALUE = ARG_IMM | ARG_REG | ARG_MEM,
	ARG_PATH  = ARG_REG | ARG_MEM,
	ARG_LABEL = ARG_IMM | ARG_MEM
};