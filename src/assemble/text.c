#include <stdlib.h>
#include "text.h"

int seekFileSize(FILE* file)
{
	fseek(file, 1L, SEEK_END);
	int length = ftell(file);
	fseek(file, 0L, SEEK_SET);

	return length - 1;
}

char* getText(FILE* file)
{
	if (file == NULL)
		return NULL;

	int size, lines = 0;

	size = seekFileSize(file);

	char* text = (char*)calloc(size + 1, 1);
	if (!text)
	{
		free(text);
		return NULL;
	}

	fread(text, 1, size, file);

	if (ferror(file))
	{
		free(text);
		return NULL;
	}

	return text;
}

int scanStr(char** stream, char* inp)
{
	int n = -1;
	int st = sscanf(*stream, "%s%n", inp, &n);
	*stream += n;
	return st;
}