#include "config.h"
#include "commands.h"

size_t getFileSize(FILE * file); // get size of unread part of FILE file in bytes

int readCode(FILE * file, cell_t ** code, int * size); // allcate memory for code and fill it from FILE file
