#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../config.h"
#include "../commands.h"
#include "name_table.h"
#include "text.h"

struct JumpRequest
{
	int row;
	char* label;
};

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

char* prepareText(char* raw_text)
{
	int size = strlen(raw_text);
	char* text = (char*)calloc(size + 1, sizeof(char));
	if (!text) 
		return NULL;

	char* text_p = text;

	int is_comment = 0;

	while (*raw_text != '\0')
	{
		if (*raw_text == ';')
			is_comment = 1;
		else if (*raw_text == '\n')
			is_comment = 0;

		if (!is_comment)
			*text_p++ = *raw_text++;
		else raw_text++;
	}

	return text;
}

int parseArgs(char ** text, cell_t * imm, cell_t * reg)
{
    const int ARG_MAX_LENGTH = 50;
    char argument_mem[MAX_CODE_SIZE] = { 0 };
    char* arg = argument_mem;

    if (scanStr(text, arg) == 1)
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

int compile(char * text, uint8_t* data, NameTable * label_names, JumpRequest ** jumps, size_t * njumps, FILE* fout)
{
    int command_bytes = MEM_FRIENDLY ? 1 : CELL_SIZE;
	size_t jumps_capacity = 0;

    char command_str[MAX_CODE_SIZE];
    double argument;

    if (!data)
    {
        fprintf(stderr, "File reading error: Couldn't allocate %d bytes\n", MAX_CODE_SIZE);
        return MEM_ERROR;
    }

    int code_size = 0;
    int unify_size = 0;

    while (scanStr(&text, command_str) == 1)
    {
        int found_command = 0;
        for (cell_t cur = 0; cur < NCOMMANDS; cur++)
        {
            if (strcmp(command_str, COMMAND_NAME[cur]))
                continue;

            uint64_t command = COMMAND_CODE[cur];
			uint64_t out_cmd = command;

            cell_t imm = -1;
			cell_t reg = -1;


            if ((command & ARG_ALL) == ARG_VALUE || (command & ARG_ALL) == ARG_PATH)
            {
                int read_code = parseArgs(&text, &imm, &reg);
                if (read_code < 0)
                    return ARG_ERROR;
                else if (read_code == 0)
                {
                    fprintf(stderr, "Compilation error: command '%s' must have an argument\n", command_str);
                    return ARG_ERROR;
                }

				if (command & ARG_MEM)
				{
					command |= ARG_REG;
					command |= ARG_IMM;
				}

                if (read_code & command != read_code)
                {
                    if (command & ARG_ALL == ARG_IMM)
                        fprintf(stderr, "Compilation error: an agrument of command '%s' must be a constant\n", command_str);

                    else if (command & ARG_ALL == ARG_REG)
                        fprintf(stderr, "Compilation error: an agrument of command '%s' must be a regex name only\n", command_str);

                    else if ((command & ARG_MEM) == 0)
                        fprintf(stderr, "Compilation error: an agrument of command '%s' mustn't work with RAM\n", command_str);

                    return ARG_ERROR;
                }

                command &= (read_code | ARG_CMD);
				

				if (pushBytes(data, &code_size, (uint8_t*)& command, command_bytes))
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
            }
			else if ((command & ARG_ALL) == ARG_LABEL)
			{
				if (scanStr(&text, command_str) != 1)
				{
					fprintf(stderr, "Compilation error: invalid jump argument\n");
					return READ_ERROR;
				}

				if (jumps_capacity == *njumps)
				{
					jumps_capacity = (jumps_capacity ? jumps_capacity * 2 : 1);

					*jumps = (JumpRequest*)realloc(*jumps, jumps_capacity * sizeof(JumpRequest));
					if (!jumps)
					{
						fprintf(stderr, "Compilation error: couldn't allocate %d bytes\n", jumps_capacity * sizeof(JumpRequest));
						return MEM_ERROR;
					}
				}

				(*jumps)[*njumps].label = (char*)calloc(strlen(command_str) + 1, sizeof(char));

				if (!(*jumps)[*njumps].label)
				{
					fprintf(stderr, "Compilation error: couldn't allocate memory\n");
					return MEM_ERROR;
				}

				(*jumps)[*njumps].label = strcpy((*jumps)[*njumps].label, command_str);

				(*njumps)++;



				if (pushBytes(data, &code_size, (uint8_t*)& command, command_bytes))
					return READ_ERROR;
				unify_size += CELL_SIZE;

				(*jumps)[*njumps - 1].row = code_size;
				cell_t emp = -1;

				if (pushBytes(data, &code_size, (uint8_t*)& emp, CELL_SIZE))
					return READ_ERROR;
				unify_size += CELL_SIZE;
			}
			else if ((command & ARG_ALL) == ARG_NOARG)
			{
				if (pushBytes(data, &code_size, (uint8_t*)& command, command_bytes))
					return READ_ERROR;
				unify_size += CELL_SIZE;
			}

            found_command = 1;
            break;
        }

		if (!found_command)
		{
			if (command_str[strlen(command_str) - 1] != ':')
			{
				fprintf(stderr, "Compilation error: Unknown command '%s'\n", command_str);
				return WRONG_COMMAND;
			}

			command_str[strlen(command_str) - 1] = '\0';
			int st = NameTableSetValue(label_names, command_str, unify_size / CELL_SIZE);

			if (st < 0)
			{
				fprintf(stderr, "Compilation error: Name table error. Code: %d\n", st);
				return st;
			}
			if (st > 1)
			{
				fprintf(stderr, "Compilation error: Double declaration of label '%s'\n", command_str);
				return TABLE_ERROR;
			}
        }
    }

	for (int i = 0; i < *njumps; ++i)
	{
		ntval_t buf;
		int st = NameTableGetValue(label_names, (*jumps)[i].label, &buf);

		if (st != 1)
		{
			fprintf(stderr, "Compilation error: couldn't find label '%s'\n", (*jumps)[i].label);
			return ARG_ERROR;
		}

		cell_t dest = buf;

		int from = (*jumps)[i].row;
		if (pushBytes(data, &from, (uint8_t*)& dest, CELL_SIZE))
			return READ_ERROR;
	}



    int out_error = 0;

    if (fwrite(SIGNATURE, 4, 1, fout) != 1)
        out_error++;

    if (fwrite(&VERSION, 3, 1, fout) != 1)
        out_error++;

    if (fwrite(&MEM_FRIENDLY, 1, 1, fout) != 1)
        out_error++;

    if (fwrite(&unify_size, 4, 1, fout) != 1)
        out_error++;

    if (fwrite(data, 1, code_size, fout) != code_size)
        out_error++;

    if (out_error)
    {
        fprintf(stderr, "Compilation error: Can't write code to the file\n");
        return WRITE_ERROR;
    }

    return 0;
}

FILE* getFile(const char* path, const char* mode, const char* err)
{
	FILE* file = fopen(path, mode);

	if (!file)
	{
		fprintf(stderr, err);
		return NULL;
	}

	return file;
}

int main(int arc, char * argv[])
{
	FILE* fin = stdin;
	FILE* fout = stdout;
	if (arc > 1)
		fin = getFile(argv[1], "r", "Compilation error: Can't open input file");
	if (arc > 2)
		fout = getFile(argv[2], "wb", "Compilation error: Can't open output file");

	if (!fin || !fout)
		return 0;

	char* main_text = getText(fin);

	if (main_text)
	{
		char* prep = prepareText(main_text);
		free(main_text);
		main_text = prep;
	}

	if (!main_text)
	{
		fprintf(stderr, "Compilation error: Can't read input file\nCompiler aborted!\n");
		return 0;
	}

	

    uint8_t * data = (uint8_t*)calloc(MAX_CODE_SIZE, 1);

	JumpRequest* jumps = NULL;
	size_t njumps = 0;

	NameTable label_names;
	NameTableCtor(&label_names);



    if (compile(main_text, data, &label_names, &jumps, &njumps, fout))
        fprintf(stderr, "Compiler aborted!\n");

	free(main_text);
	free(data);

	while (njumps--)
		free(jumps[njumps].label);
	free(jumps);

	NameTableDtor(&label_names);

	return 0;
}
