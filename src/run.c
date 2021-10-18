#include <stdio.h>
#include <stdlib.h>
#include "code_utils.h"
#include "../../MyStack/stack.h"

cell_t getArgument(cell_t * code, int size, int * counter, cell_t* arg)
{
    if (*counter == size)
    {
        printf("Runtime error: code corrupted. Please recompile\n");
        return ARG_ERROR;
    }

    *arg = code[(*counter)++];
    return 0;
}

int run(cell_t * code, Stack * code_stack, int program_size)
{
    int counter = 0;

    while (counter < program_size && code[counter] != 0)
    {
        switch(code[counter++])
        {
            case ASM_HLT:
                return 0;

            case ASM_PUSH:
            {
                cell_t arg;
                if (getArgument(code, program_size, &counter, &arg))
                    return ARG_ERROR;

                if (StackPush(code_stack, &arg))
                    return STACK_ERROR;
                break;
            }

            case ASM_CUSH:
            {
                cell_t arg;
                scanf("%ld", &arg);

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

    int err = run(code, &code_stack, size);
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
