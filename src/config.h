#pragma once

#include <stdint.h>

extern const char SIGNATURE[5]; //current version of code
extern const int VERSION; //current version of code
extern const int MAX_CODE_SIZE; // bytes

extern const int MEM_FRIENDLY;
// shows if the code is equal-length. If flag is 1, each command codes with one byte

extern const int CELL_SIZE;
// Cell is a memory used by single item of a program

extern const char CELL_FORMAT[3];
// Format for input or output of cell

extern const int REGISTER_NUMBER;
// Number of registers

extern const int SOFT_RAM_SIZE;
// Size of soft ram in bytes

typedef int32_t cell_t;

enum ERRORS {
	WRONG_COMMAND = -1,
	ARG_ERROR = -2,
	READ_ERROR = -3,
	WRITE_ERROR = -4,
	MEM_ERROR = -5,
	STACK_ERROR = -6,
	TABLE_ERROR = -7
};
