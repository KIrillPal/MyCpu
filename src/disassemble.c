#include <stdio.h>
#include <stdlib.h>
#include "code_utils.h"

int decompile(cell_t * code, int program_size)
{
    unsigned int counter = 0;

    for (counter = 0; counter < program_size;)
    {
        int command = code[counter++];

        if (command < 0 || command >= NCOMMANDS)
        {
            fprintf(stderr, "Decompilation error: unknown command code '%d'\n", command);
            return WRONG_COMMAND;
        }

        printf("%s", COMMAND_NAME[command]);

        if (counter + COMMAND_ARG_NUMBER[command] > program_size)
        {
            fprintf(stderr, "Decompilation error: code corrupted. Please recompile\n");
            return ARG_ERROR;
        }

        int args = 0;

        while (args < COMMAND_ARG_NUMBER[command])
        {
            printf(" %ld", code[counter++]);
            args++;
        }

        printf("\n");
    }

    return 0;
}

int main()
{
    int size = 0;
    cell_t * code;

    if (readCode(stdin, &code, &size))
    {
        fprintf(stderr, "Decompiler aborted!\n");
        return 0;
    }

    if (decompile(code, size))
    {
        fprintf(stderr, "Decompiler aborted!\n");
        free(code);
        return 0;
    }

    free(code);
    return 0;
}
