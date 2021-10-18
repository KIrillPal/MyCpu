#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "config.h"
#include "commands.h"

int pushBytes(uint8_t * data, int * data_size, uint8_t * item, int item_size)
{
    if (*data_size + item_size > MAX_CODE_SIZE)
    {
        fprintf(stderr, "Compilation error: ");
        fprintf(
            stderr,
            "Binary code file reached max size: %d bytes.\nIncrease MAX_CODE_SIZE variable in configure file \"config.h\"",
            MAX_CODE_SIZE);
        return READ_ERROR;
    }

    for (int i = 0; i < item_size; ++i)
    {
        data[(*data_size)++] = *item++;
    }
    return 0;
}

int compile()
{
    int command_bytes = MEM_FRIENDLY ? 1 : CELL_SIZE;

    char command[MAX_COMMAND_LENGTH];
    double argument;

    uint8_t * data = (uint8_t*)calloc(MAX_CODE_SIZE, 1);

    if (!data)
    {
        fprintf(stderr, "File reading error: Unable to allocate %d bytes\n", MAX_CODE_SIZE);
        return MEM_ERROR;
    }

    int code_size = 0;
    int unify_size = 0;

    while (scanf("%s", &command) == 1)
    {
        int found_command = 0;
        for (cell_t cur = 0; cur < NCOMMANDS; cur++)
        {
            if (strcmp(command, COMMAND_NAME[cur]))
                continue;

            if(pushBytes(data, &code_size, (uint8_t*)&cur, command_bytes))
            {
                free(data);
                return READ_ERROR;
            }
            unify_size += CELL_SIZE;

            int args = 0;
            while (args < COMMAND_ARG_NUMBER[cur])
            {
                cell_t arg;
                if (scanf("%ld", &arg) != 1)
                {
                    fprintf(stderr, "Compilation error: ");
                    fprintf(
                        stderr,
                        "Too few arguments in command '%s'. Excpected %d arguments. Got %d\n",
                        command,
                        COMMAND_ARG_NUMBER[cur],
                        args);

                    free(data);
                    return ARG_ERROR;
                }

                if(pushBytes(data, &code_size, (uint8_t*)&arg, CELL_SIZE))
                {
                    free(data);
                    return READ_ERROR;
                }
                unify_size += CELL_SIZE;

                args++;
            }

            found_command = 1;
            break;
        }

        if (!found_command)
        {
            fprintf(stderr, "Compilation error: ");
                    fprintf(
                        stderr,
                        "Unknown command '%s'\n",
                        command);

        }
    }

    int out_error = 0;

    if (fwrite(SIGNATURE, 4, 1, stdout) != 1)
        out_error++;

    if (fwrite(&VERSION, 3, 1, stdout) != 1)
        out_error++;

    if (fwrite(&MEM_FRIENDLY, 1, 1, stdout) != 1)
        out_error++;

    if (fwrite(&unify_size, 4, 1, stdout) != 1)
        out_error++;

    if (fwrite(data, 1, code_size, stdout) != code_size)
        out_error++;

    if (out_error)
    {
        fprintf(stderr, "Compilation error: Can't write code to the file\n");

        free(data);
        return WRITE_ERROR;
    }
    free(data);

    return 0;
}

int main()
{
    if (compile())
        fprintf(stderr, "Compiler aborted!\n");
    
    return 0;
}
