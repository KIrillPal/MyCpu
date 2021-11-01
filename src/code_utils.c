#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "code_utils.h"

 size_t getFileSize(FILE * file)
{
    long long file_cur = ftell(file);

    fseek(file, 1L, SEEK_END);
    int file_end = ftell(file);

    fseek(file, file_cur, SEEK_SET);

    return file_end - file_cur - 1;
}

int readCode(FILE * file, cell_t ** code, int * size)
{
    *code = NULL;
    *size = 0;

    char signature[5] = { 0 };
    int version = 0, mem_friendly = 0;

    int program_size;

    fread(signature    , 4, 1, file);
    fread(&version     , 3, 1, file);
    fread(&mem_friendly, 1, 1, file);
    fread(&program_size, 4, 1, file);

    int data_size = getFileSize(file);

    uint8_t* raw_data = (uint8_t*)calloc(data_size, 1);



    if (!raw_data)
    {
        fprintf(stderr, "File reading error: Unable to allocate %d bytes\n", data_size);
        return MEM_ERROR;
    }

    fread(raw_data, 1, data_size, file);

    if (ferror(file))
    {
        fprintf(stderr, "File reading error: Can't read code from the file\n");
        return READ_ERROR;
    }

    if (strcmp(signature, SIGNATURE))
    {
        fprintf(stderr, "File reading error: Binary file must have '%s' format\n", SIGNATURE);
        return READ_ERROR;
    }

    if (version != VERSION)
    {
        fprintf(stderr, "File reading error: Incompatible code version '%d'\n", VERSION);
        return READ_ERROR;
    }

    if (program_size > CELL_SIZE * MAX_CODE_SIZE || program_size % CELL_SIZE != 0)
    {
        fprintf(stderr, "File reading error: Invalid file format. Please recompile\n");
        return READ_ERROR;
    }

    if (mem_friendly)
    {
        cell_t * unify = (cell_t*)calloc(program_size, 1);

        if (!unify)
        {
            fprintf(stderr, "File reading error: Unable to allocate %d bytes\n", program_size);
            free(raw_data);
            return MEM_ERROR;
        }

        int unify_size = 0;

        for (int sz = 0; sz < data_size;)
        {
            cell_t command = raw_data[sz++];
            unify[unify_size++] = command;

			int need_space = 0;
			need_space += ((command & ARG_REG) ? CELL_SIZE : 0);
			need_space += ((command & ARG_IMM) ? CELL_SIZE : 0);

			if (sz + need_space > data_size)
			{
				fprintf(stderr, "File reading error: Invalid code format. Please recompile\n");
				free(raw_data);
				free(unify);
				return READ_ERROR;
			}

			if (command & ARG_REG)
			{
				unify[unify_size++] = *((cell_t*)(raw_data + sz));

				sz += CELL_SIZE;
			}

			if (command & ARG_IMM)
			{
				unify[unify_size++] = *((cell_t*)(raw_data + sz));

				sz += CELL_SIZE;
			}
        }

        free(raw_data);
        *code = unify;
        *size = unify_size;

        return 0;
    }

    *code = (cell_t * )raw_data;
    *size = program_size / CELL_SIZE;

    return 0;
}
