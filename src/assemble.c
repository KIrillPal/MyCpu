#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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

int parseArgs(cell_t * imm, cell_t * reg)
{
    const int ARG_MAX_LENGTH = 50;
    char argument_mem[MAX_CODE_SIZE] = { 0 };
    char* arg = argument_mem;

    if (scanf("%s", arg) == 1)
    {
        int len = strlen(arg);

        if (strspn(arg, "[abcdefghijklmnopqrstuvwxyz+-0123456789]") == len)
        {
            int read_code = 0;

            if (arg[0] == '[' && arg[len - 1] == ']')
            {
                read_code |= ARG_MEM;
                arg[0] = arg[len - 1] = '\0';
                arg++;
                len -= 2;
            }

            char arg_reg[ARG_MAX_LENGTH] = { 0 };
            cell_t res_imm;
            char div_chr = 0, end_chr = 0;

            int was_inp = 0;

            int has_reg = 0;
            int has_div = 0;
            int has_imm = 0;

            if (sscanf(arg, "%[a-z]%c%u%c", arg_reg, &div_chr, &res_imm, &end_chr) == 3)
            {
                if (end_chr == 0 && (div_chr == '+' || div_chr == '-'))
                {
                    was_inp = 1;
                    has_reg = 1;
                    has_div = 1;
                    has_imm = 1;
                }
            }
            end_chr = 0;

            if (!was_inp && sscanf(arg, "%[a-z]%c", arg_reg, &end_chr) == 1)
            {
                if (end_chr == 0)
                {
                    was_inp = 1;
                    has_reg = 1;
                }
            }
            end_chr = 0;

            if (!was_inp && sscanf(arg, "%d%c", &res_imm, &end_chr) == 1)
            {
                if (end_chr == 0)
                {
                    was_inp = 1;
                    has_imm = 1;
                }
            }

            if (has_reg || has_imm)
            {
                if (has_reg)
                {
                    int err = 0;
                    if (strlen(arg_reg) != 2)
                        err = ARG_ERROR;
                    else if (arg_reg[0] < 'a' || 'a' + REGISTER_NUMBER <= arg_reg[0])
                        err = ARG_ERROR;
                    else if (arg_reg[1] != 'x')
                        err = ARG_ERROR;

                    if (err)
                    {
                        fprintf(stderr, "Invalid register \"%s\". It must be only \"<a-%c>x\" format\n", arg_reg, 'a' + REGISTER_NUMBER - 1);
                        return ARG_ERROR;
                    }

                    *reg = arg_reg[0] - 'a';
                    read_code |= ARG_REG;
                }

                if (has_imm)
                {
                    if (has_div && div_chr == '-')
                        res_imm *= -1;

                    *imm = res_imm;
                    read_code |= ARG_IMM;
                }

                return read_code;
            }
        }

        fprintf(stderr, "Invalid argument \"%s\"\n", argument_mem);
        return ARG_ERROR;
    }

    return 0;
}

int compile(uint8_t* data)
{
    int command_bytes = MEM_FRIENDLY ? 1 : CELL_SIZE;

    char command_str[MAX_CODE_SIZE];
    double argument;

    if (!data)
    {
        fprintf(stderr, "File reading error: Unable to allocate %d bytes\n", MAX_CODE_SIZE);
        return MEM_ERROR;
    }

    int code_size = 0;
    int unify_size = 0;

    while (scanf("%s", &command_str) == 1)
    {
        int found_command = 0;
        for (cell_t cur = 0; cur < NCOMMANDS; cur++)
        {
            if (strcmp(command_str, COMMAND_NAME[cur]))
                continue;

            uint64_t command = COMMAND_CODE[cur];

            cell_t imm = -1;
            cell_t reg = -1;


            if (command & ARG_ALL)
            {

                int read_code = parseArgs(&imm, &reg);
                if (read_code < 0)
                    return ARG_ERROR;
                else if (read_code == 0)
                {
                    fprintf(stderr, "Compilation error: command '%s' must have an argument\n", command_str);
                    return ARG_ERROR;
                }

                if (read_code & command != read_code)
                {
                    if (command & ARG_ALL == ARG_IMM)
                        fprintf(stderr, "Compilation error: an agrument of command '%s' must be a constant\n", command_str);

                    else if (command & ARG_ALL == ARG_REG)
                        fprintf(stderr, "Compilation error: an agrument of command '%s' must be a regex name only\n", command_str);

                    else if (command & ARG_MEM == 0)
                        fprintf(stderr, "Compilation error: an agrument of command '%s' mustn't work with RAM\n", command_str);

                    return ARG_ERROR;
                }

                command &= (read_code | ARG_CMD);
            }

            if (pushBytes(data, &code_size, (uint8_t*) &command, command_bytes))
                return READ_ERROR;
            unify_size += CELL_SIZE;

            if (command & ARG_REG)
            {
                if (pushBytes(data, &code_size, (uint8_t*)& reg, CELL_SIZE))
                    return READ_ERROR;
                unify_size += CELL_SIZE;
            }

            if (command & ARG_IMM)
            {
                if (pushBytes(data, &code_size, (uint8_t*)& imm, CELL_SIZE))
                    return READ_ERROR;
                unify_size += CELL_SIZE;
            }

            found_command = 1;
            break;
        }

        if (!found_command)
        {
            fprintf(stderr, "Compilation error: Unknown command '%s'\n", command_str);
            return WRONG_COMMAND;
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
        return WRITE_ERROR;
    }
    return 0;
}

int main()
{
    uint8_t * data = (uint8_t*)calloc(MAX_CODE_SIZE, 1);

    if (compile(data))
        fprintf(stderr, "Compiler aborted!\n");

    free(data);
    return 0;
}
