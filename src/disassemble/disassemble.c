#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "../code_utils.h"

struct SomeLabel
{
	unsigned int line;
	unsigned int jump;
};

int someLabelComp(const void* first, const void* second)
{
	return ((SomeLabel*)first)->line - ((SomeLabel*)second)->line;
}

void printLabelName(unsigned int label, FILE * fout)
{
	if (label == 0)
	{
		fprintf(fout, "A");
		return;
	}

	const int MAX_LABEL_SZ = 8;
	char label_name[MAX_LABEL_SZ] = { 0 };

	int i;
	for (i = 0; i < MAX_LABEL_SZ && label > 0; ++i)
	{
		label_name[i] = 'A' + label % 26;
		label /= 26;
	}

	for (; i > 0; --i)
		fprintf(fout, "%c", label_name[i - 1]);
}

int decompile(cell_t * code, int program_size, FILE * fout)
{
    unsigned int counter = 0;

	int  ll_size = 0, ll_cap = 8;
	SomeLabel* labels = (SomeLabel*)calloc(ll_cap, sizeof(SomeLabel));

	for (counter = 0; counter < program_size;)
	{
		int command = code[counter++];
		int cmd_number = command & ARG_CMD;

		if (cmd_number < 0 || cmd_number >= NCOMMANDS)
		{
			fprintf(stderr, "Decompilation error: unknown command code '%d'\n", cmd_number);
			return WRONG_COMMAND;
		}


		if ((COMMAND_CODE[cmd_number] & ARG_ALL) == ARG_LABEL)
		{
			if (ll_cap == ll_size)
			{
				ll_cap = (ll_cap ? 2 * ll_cap : 1);
				labels = (SomeLabel*)realloc(labels, ll_cap * sizeof(SomeLabel));
			}

			labels[ll_size].jump = counter;
			labels[ll_size].line = code[counter++];
			ll_size++;
		}
		else
		{
			counter += (command & ARG_REG) != 0;
			counter += (command & ARG_IMM) != 0;
		}
	}
	
	qsort(labels, ll_size, sizeof(SomeLabel), someLabelComp);

	int label_order = 0, ll_ptr = 0;

	for (int i = 0; i < ll_size; ++i)
	{
		if (i > 0 && labels[i].line != labels[i - 1].line)
			++label_order;

		code[labels[i].jump] = label_order;
	}							

	for (counter = 0; counter < program_size;)
	{
		printf("go %d\n", counter);
		if (ll_ptr < ll_size && labels[ll_ptr].line == counter)
		{
			fprintf(fout, "\n");
			printLabelName(code[labels[ll_ptr].jump], fout);
			fprintf(fout, ":\n\n");

			while (ll_ptr < ll_size && labels[ll_ptr].line == counter)
				ll_ptr++;
		}

		int command = code[counter++];
		int cmd_number = command & ARG_CMD;

		fprintf(fout, "%s ", COMMAND_NAME[cmd_number]);

		if ((COMMAND_CODE[cmd_number] & ARG_ALL) == ARG_LABEL)
		{
			printf("to %s\n", COMMAND_NAME[cmd_number]);
			printLabelName(code[counter++], fout);
		}
		else
		{
			if (command & ARG_MEM)
				fprintf(fout, "[");

			if (command & ARG_REG)
			{
				char reg_symbol = 'a' + code[counter++];
				printf("ready %c\n", reg_symbol);
				char out_reg[3] = { reg_symbol, 'x', '\0' };
				fprintf(fout, "%s", out_reg);
			}

			if (command & ARG_REG && command & ARG_IMM)
				fprintf(fout, "+");

			if (command & ARG_IMM)
				fprintf(fout, "%d", code[counter++]);

			if (command & ARG_MEM)
				fprintf(fout, "]");
		}
		
        fprintf(fout, "\n");
    }

	free(labels);

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

int main(int arc, char* argv[])
{
	FILE* fin = stdin;
	FILE* fout = stdout;
	if (arc > 1)
		fin = getFile(argv[1], "rb", "Decompilation error: Can't open input file");
	if (arc > 2)
		fout = getFile(argv[2], "w", "Decompilation error: Can't open output file");

	if (!fin || !fout)
		return 0;

    int size = 0;
    cell_t * code;

    if (readCode(fin, &code, &size))
    {
        fprintf(stderr, "Decompiler aborted!\n");
        return 0;
    }

    if (decompile(code, size, fout))
    {
        fprintf(stderr, "Decompiler aborted!\n");
        free(code);
        return 0;
    }

    free(code);
    return 0;
}
