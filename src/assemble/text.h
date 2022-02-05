#pragma once
#include <stdio.h>

int seekFileSize (FILE * file);

char*  getText  (FILE * file);

int scanStr(char** stream, char* inp);