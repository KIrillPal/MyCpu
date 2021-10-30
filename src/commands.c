#include "commands.h"

const char COMMAND_NAME[NCOMMANDS][MAX_COMMAND_LENGTH] = {
    "HLT"
    "PUSH",
    "POP",
    "ADD",
    "SUB",
    "MUL",
    "DEL",
    "DUPE",
    "OUT",
    "JMP"
};

const int COMMAND_CODE[NCOMMANDS] = {
	ASM_HLT,
	ASM_PUSH | ARG_VALUE,
	ASM_POP  | ARG_PATH,
	ASM_ADD,
	ASM_SUB,
	ASM_MUL,
	ASM_DEL,
	ASM_DUPE,
	ASM_OUT,
    ASM_JMP  | ARG_LABEL
};
