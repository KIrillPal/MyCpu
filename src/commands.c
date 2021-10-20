#include "commands.h"

const char COMMAND_NAME[NCOMMANDS][MAX_COMMAND_LENGTH] = {
    "HLT",
    "PUSH",
    "POP",
    "ADD",
    "SUB",
    "MUL",
    "DEL",
    "DUPE",
    "OUT"
};

const int COMMAND_CODE[NCOMMANDS] = {
	ASM_HLT,
	ASM_PUSH | ARG_IMM | ARG_REG,
	ASM_POP  | ARG_REG,
	ASM_ADD,
	ASM_SUB,
	ASM_MUL,
	ASM_DEL,
	ASM_DUPE,
	ASM_OUT
};
