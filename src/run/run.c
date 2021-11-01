#include <stdio.h>
#include <stdlib.h>
#include "../code_utils.h"
#include "../../../MyStack/src/stack.h"

cell_t getArgument(cell_t * code, cell_t * regs, cell_t * ram, int * counter, cell_t command, cell_t* arg)
{
	cell_t val = 0;

	if (command & ARG_REG)
	{
		cell_t pos = code[(*counter)++];
		if (pos < 0 || pos >= REGISTER_NUMBER)
		{
			printf("Runtime error: Invalid register index: %d\n", pos);
			return ARG_ERROR;
		}

		val += regs[pos];
	}

	if (command & ARG_IMM)
	{
		val += code[(*counter)++];
	}

	if (command & ARG_MEM)
	{
		if (val < 0 || (val + 1) * CELL_SIZE > SOFT_RAM_SIZE)
		{
			printf("Runtime error: Invalid ram pointer: %d byte\n", val * CELL_SIZE);
			return ARG_ERROR;
		}
		val = ram[val];
	}

	*arg = val;
	return 0;
}

cell_t getPath(cell_t* code, cell_t* regs, cell_t* ram, int* counter, cell_t command, cell_t** arg)
{
	if (!(command & ARG_IMM) && !(command & ARG_REG))
	{
		printf("Runtime error: Invalid pop argument. Please recompile!\n");
		return ARG_ERROR;
	}

	if ((command & ARG_IMM) && !(command & ARG_MEM))
	{
		printf("Runtime error: Invalid pop argument. Please recompile!\n");
		return ARG_ERROR;
	}

	cell_t* ptr = NULL;
	cell_t imm = 0;

	if (command & ARG_REG)
	{
		cell_t pos = code[(*counter)++];
		if (pos < 0 || pos >= REGISTER_NUMBER)
		{
			printf("Runtime error: Invalid register index: %d\n", pos);
			return ARG_ERROR;
		}

		ptr = regs + pos;
	}

	if (command & ARG_IMM)
	{
		imm = code[(*counter)++];
	}

	if (command & ARG_MEM)
	{
		cell_t val;
		if (command & ARG_REG)
			val = *ptr + imm;
		else val = imm;
		if (val < 0 || (val + 1) * CELL_SIZE > SOFT_RAM_SIZE)
		{
			printf("Runtime error: Invalid ram pointer: %d byte\n", val * CELL_SIZE);
			return ARG_ERROR;
		}
		ptr = ram + val;
	}

	if (ptr == NULL)
	{
		printf("Runtime error: Invalid pop argument. Please recompile!\n");
		return ARG_ERROR;
	}

	*arg = ptr;
	return 0;
}

int run(cell_t * code, cell_t * regs, cell_t * ram, Stack * code_stack, Stack* ret_stack, int program_size)
{
	int counter = 0;

    while (counter < program_size && code[counter] != 0)
    {
		cell_t command = code[counter++];

		switch(command & ARG_CMD)
        {
            case ASM_HLT:
                return 0;

            case ASM_PUSH:
            {
                cell_t arg;
                if (getArgument(code, regs, ram, &counter, command, &arg))
                    return ARG_ERROR;

                if (StackPush(code_stack, &arg))
                    return STACK_ERROR;
                break;
            }

            case ASM_POP:
            {
                cell_t val;
                if (StackPop(code_stack, &val))
                    return STACK_ERROR;

				cell_t* ptr;
				if (getPath(code, regs, ram, &counter, command, &ptr))
					return ARG_ERROR;

				*ptr = val;
                break;
            }

			case ASM_IN:
			{
				cell_t arg;
				scanf(CELL_FORMAT, &arg);

				if (StackPush(code_stack, &arg))
					return STACK_ERROR;
				break;
			}

            case ASM_ADD:
            {
                cell_t arg1, arg2;

                if (StackPop(code_stack, &arg2))
                    return STACK_ERROR;

                if (StackPop(code_stack, &arg1))
                    return STACK_ERROR;

                cell_t arg3 = arg1 + arg2;

                if (StackPush(code_stack, &arg3))
                    return STACK_ERROR;
                break;
            }

            case ASM_SUB:
            {
                cell_t arg1, arg2;

                if (StackPop(code_stack, &arg2))
                    return STACK_ERROR;

                if (StackPop(code_stack, &arg1))
                    return STACK_ERROR;

                cell_t arg3 = arg1 - arg2;

                if (StackPush(code_stack, &arg3))
                    return STACK_ERROR;
                break;
            }

            case ASM_MUL:
            {
                cell_t arg1, arg2;

                if (StackPop(code_stack, &arg2))
                    return STACK_ERROR;

                if (StackPop(code_stack, &arg1))
                    return STACK_ERROR;

                cell_t arg3 = arg1 * arg2;

                StackPush(code_stack, &arg3);
                break;
            }

            case ASM_DEL:
            {
                cell_t arg1, arg2;

                if (StackPop(code_stack, &arg2))
                    return STACK_ERROR;

                if (StackPop(code_stack, &arg1))
                    return STACK_ERROR;

                cell_t arg3 = arg1 / arg2;

                StackPush(code_stack, &arg3);
                break;
            }

            case ASM_DUPE:
            {
                cell_t arg;

                if (StackTop(code_stack, &arg))
                    return STACK_ERROR;

                if (StackPush(code_stack, &arg))
                    return STACK_ERROR;

                break;
            }

            case ASM_OUT:
            {
                cell_t arg;

                if (StackPop(code_stack, &arg))
                    return STACK_ERROR;

				printf(CELL_FORMAT, arg);
				printf("\n");
                break;
            }

			case ASM_JMP:
			{
				cell_t arg;
				if (getArgument(code, regs, ram, &counter, ARG_IMM, &arg))
					return ARG_ERROR;

				if (arg < 0 || arg >= program_size)
				{
					printf("Runtime error: Invalid jump pointer: %d\n", arg);
					return ARG_ERROR;
				}

				counter = arg;
				break;
			}

			case ASM_JE:
			{
				cell_t arg;
				if (getArgument(code, regs, ram, &counter, ARG_IMM, &arg))
					return ARG_ERROR;

				if (arg < 0 || arg >= program_size)
				{
					printf("Runtime error: Invalid jump pointer: %d\n", arg);
					return ARG_ERROR;
				}

				cell_t arg1, arg2;

				if (StackPop(code_stack, &arg2))
					return STACK_ERROR;

				if (StackPop(code_stack, &arg1))
					return STACK_ERROR;

				if (arg1 == arg2)
					counter = arg;
				break;
			}

			case ASM_JNE:
			{
				cell_t arg;
				if (getArgument(code, regs, ram, &counter, ARG_IMM, &arg))
					return ARG_ERROR;

				if (arg < 0 || arg >= program_size)
				{
					printf("Runtime error: Invalid jump pointer: %d\n", arg);
					return ARG_ERROR;
				}

				cell_t arg1, arg2;

				if (StackPop(code_stack, &arg2))
					return STACK_ERROR;

				if (StackPop(code_stack, &arg1))
					return STACK_ERROR;

				if (arg1 != arg2)
					counter = arg;
				break;
			}

			case ASM_JL:
			{
				cell_t arg;
				if (getArgument(code, regs, ram, &counter, ARG_IMM, &arg))
					return ARG_ERROR;

				if (arg < 0 || arg >= program_size)
				{
					printf("Runtime error: Invalid jump pointer: %d\n", arg);
					return ARG_ERROR;
				}

				cell_t arg1, arg2;

				if (StackPop(code_stack, &arg2))
					return STACK_ERROR;

				if (StackPop(code_stack, &arg1))
					return STACK_ERROR;

				if (arg1 < arg2)
					counter = arg;
				break;
			}

			case ASM_JLE:
			{
				cell_t arg;
				if (getArgument(code, regs, ram, &counter, ARG_IMM, &arg))
					return ARG_ERROR;

				if (arg < 0 || arg >= program_size)
				{
					printf("Runtime error: Invalid jump pointer: %d\n", arg);
					return ARG_ERROR;
				}

				cell_t arg1, arg2;

				if (StackPop(code_stack, &arg2))
					return STACK_ERROR;

				if (StackPop(code_stack, &arg1))
					return STACK_ERROR;

				if (arg1 <= arg2)
					counter = arg;
				break;
			}

			case ASM_JG:
			{
				cell_t arg;
				if (getArgument(code, regs, ram, &counter, ARG_IMM, &arg))
					return ARG_ERROR;

				if (arg < 0 || arg >= program_size)
				{
					printf("Runtime error: Invalid jump pointer: %d\n", arg);
					return ARG_ERROR;
				}

				cell_t arg1, arg2;

				if (StackPop(code_stack, &arg2))
					return STACK_ERROR;

				if (StackPop(code_stack, &arg1))
					return STACK_ERROR;

				if (arg1 > arg2)
					counter = arg;
				break;
			}

			case ASM_JGE:
			{
				cell_t arg;
				if (getArgument(code, regs, ram, &counter, ARG_IMM, &arg))
					return ARG_ERROR;

				if (arg < 0 || arg >= program_size)
				{
					printf("Runtime error: Invalid jump pointer: %d\n", arg);
					return ARG_ERROR;
				}

				cell_t arg1, arg2;

				if (StackPop(code_stack, &arg2))
					return STACK_ERROR;

				if (StackPop(code_stack, &arg1))
					return STACK_ERROR;

				if (arg1 >= arg2)
					counter = arg;
				break;
			}

			case ASM_CALL:
			{
				cell_t arg;
				if (getArgument(code, regs, ram, &counter, ARG_IMM, &arg))
					return ARG_ERROR;

				if (arg < 0 || arg >= program_size)
				{
					printf("Runtime error: Invalid jump pointer: %d\n", arg);
					return ARG_ERROR;
				}

				if (StackPush(ret_stack, &counter))
					return STACK_ERROR;

				counter = arg;
				break;
			}

			case ASM_RET:
			{
				if (ret_stack->size == 0)
				{
					printf("Runtime error: return from main\n");
					return WRONG_COMMAND;
				}
				if (StackPop(ret_stack, &counter))
					return STACK_ERROR;
				break;
			}

            default:
                printf("Runtime error: unknown command '%ld'\n", code[counter]);
                return WRONG_COMMAND;
                break;
        }
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

int main(int arc, char* argv[])
{
	FILE* fin = NULL;
	if (arc > 1)
		fin = getFile(argv[1], "rb", "Compilation error: Can't open input file");

	if (!fin)
		return 0;

    int size = 0;
	cell_t* code;

    if (readCode(fin, &code, &size))
    {
        printf("Soft CPU aborted!\n");
        return 0;
    }

    Stack code_stack, ret_stack;
	StackCtor(&code_stack, CELL_SIZE);
	StackCtor(&ret_stack , CELL_SIZE);

	cell_t regs[REGISTER_NUMBER] = { 0 };
	cell_t * ram = (cell_t*)calloc(SOFT_RAM_SIZE, 1);


    int err = run(code, regs, ram, &code_stack, &ret_stack, size);
    if (err)
    {
        if (err == STACK_ERROR)
            printf("Runtime error: stack error\n");
        printf("Soft CPU aborted!\n");

		StackDtor(&code_stack);
		StackDtor(&ret_stack);
        free(code);

        return 0;
    }

	StackDtor(&code_stack);
	StackDtor(&ret_stack);
    free(code);
    return 0;
}
