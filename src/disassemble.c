#include <stdio.h>
#include <stdlib.h>
#include "code_utils.h"

int decompile(cell_t * code, int program_size)
{
    unsigned int counter = 0;

    for (counter = 0; counter < program_size;)
    {
        int command = code[counter++];
		int cmd_number = command & ARG_CMD;

        if (cmd_number < 0 || cmd_number >= NCOMMANDS)
        {
            fprintf(stderr, "Decompilation error: unknown command code '%d'\n", cmd_number);
            return WRONG_COMMAND;
        }

        printf("%s", COMMAND_NAME[cmd_number++]);

		if (command & ARG_MEM)
			printf("[");

		if (command & ARG_REG)
		{
			char reg_symbol = 'a' + code[counter++];
			char out_reg[3] = { reg_symbol, 'x', '\0' };
			printf("%s", out_reg);
		}

		if (command & ARG_REG && command & ARG_IMM)
			printf("+");

		if (command & ARG_IMM)
			printf("%d", code[counter++]);

		if (command & ARG_MEM)
			printf("]");

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
