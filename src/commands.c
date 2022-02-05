#include "commands.h"

const char COMMAND_NAME[NCOMMANDS][MAX_COMMAND_LENGTH] = {
    "HLT",
    "PUSH",
    "POP",
	"IN",

    "ADD",
    "SUB",
    "MUL",
    "DEL",
    "DUPE",
    "OUT",

    "JMP",
	"JE",
	"JNE",
	"JL",
	"JLE",
	"JG",
	"JGE",
	"CALL",
	"RET"
};

const int COMMAND_CODE[NCOMMANDS] = {
	ASM_HLT,
	ASM_PUSH | ARG_VALUE,
	ASM_POP  | ARG_PATH,
	ASM_IN,

	ASM_ADD,
	ASM_SUB,
	ASM_MUL,
	ASM_DEL,
	ASM_DUPE,
	ASM_OUT,

	ASM_JMP  | ARG_LABEL,
	ASM_JE   | ARG_LABEL,
	ASM_JNE  | ARG_LABEL,
	ASM_JL   | ARG_LABEL,
	ASM_JLE  | ARG_LABEL,
	ASM_JG   | ARG_LABEL,
	ASM_JGE  | ARG_LABEL,
	ASM_CALL | ARG_LABEL,
	ASM_RET
};
