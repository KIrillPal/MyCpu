#include <stdio.h>
#include <stdlib.h>
#include "code_utils.h"
#include "../../MyStack/stack.h"

cell_t getArgument(cell_t * code, cell_t * regs, int * counter, cell_t command, cell_t* arg)
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

	*arg = val;
    return 0;
}

int run(cell_t * code, cell_t * regs, Stack * code_stack, int program_size)
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
                if (getArgument(code, regs, &counter, command, &arg))
                    return ARG_ERROR;

                if (StackPush(code_stack, &arg))
                    return STACK_ERROR;
                break;
            }

            case ASM_POP:
            {
                cell_t arg;
                if (StackPop(code_stack, &arg))
                    return STACK_ERROR;

                cell_t reg;
                if (getArgument(code, regs, &counter, ARG_IMM, &reg))
                    return ARG_ERROR;

                if (reg < 0 || reg >= REGISTER_NUMBER)
                {
                    printf("Runtime error: Invalid register index: %d\n", reg);
                    return ARG_ERROR;
                }

                regs[reg] = arg;
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

                printf("%d\n", arg);
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

int main()
{
    int size = 0;
    cell_t * code;

    if (readCode(stdin, &code, &size))
    {
        printf("Soft CPU aborted!\n");
        return 0;
    }

    Stack code_stack;
    StackCtor(&code_stack, CELL_SIZE);

    cell_t regs[REGISTER_NUMBER] = { 0 };

    int err = run(code, regs, &code_stack, size);
    if (err)
    {
        if (err == STACK_ERROR)
            printf("Runtime error: stack error\n");
        printf("Soft CPU aborted!\n");

        StackDtor(&code_stack);
        free(code);

        return 0;
    }

    StackDtor(&code_stack);
    free(code);
    return 0;
}
