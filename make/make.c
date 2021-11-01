#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>

#define MAX_FILE_PATH_LENGTH 30
#define FILE_NUMBER 11

int getTime(const char* file_path, time_t* time)
{
    struct stat file_stat;
    if (stat(file_path, &file_stat))
        return -1;

    *time = file_stat.st_mtime;
    return 0;
}

int recompile(const char* file, const char* command)
{
    printf("Recompile file '%s'...\n", file);

    if (!system(NULL))
    {
        printf("Making error: terminal access is denied\n");
        return -3;
    }

    return system(command);
}

int update(const char project_files[FILE_NUMBER][MAX_FILE_PATH_LENGTH], size_t nfiles, const char* exe_file, const char* compile_command)
{
    time_t exe_mtime;

    if (getTime(exe_file, &exe_mtime))
        exe_mtime = 0;

    int i;
    for (i = 0; i < nfiles; ++i)
    {
        time_t mtime;

        if (getTime(project_files[i], &mtime))
        {
            printf("Making error: can't get date of file '%s'\n", project_files[i]);
            return -1;
        }

        if (mtime > exe_mtime)
            return recompile(exe_file, compile_command);
    }

    return 0;
}

int main(int arc, char * argv[])
{
    const char BYTECODE_FILE[] = "program.bin";

    const char ASSEMBLE_FILES[FILE_NUMBER][MAX_FILE_PATH_LENGTH] =
    {
        "../src/config.h",
        "../src/commands.h",

        "../src/config.c",
        "../src/commands.c",
        "../src/assemble.c"
    };

    const char DISASSEMBLE_FILES[FILE_NUMBER][MAX_FILE_PATH_LENGTH] =
    {
        "../src/config.h",
        "../src/commands.h",
        "../src/code_utils.h",

        "../src/config.c",
        "../src/commands.c",
        "../src/code_utils.c",
        "../src/disassemble.c"
    };

    const char RUN_FILES[FILE_NUMBER][MAX_FILE_PATH_LENGTH] =
    {
        "../src/config.h",
        "../src/commands.h",
        "../src/code_utils.h",
        "../../MyStack/stack.h",

        "../src/config.c",
        "../src/commands.c",
        "../src/code_utils.c",
        "../../MyStack/stack.c",
        "../src/run.c"
    };

    if (update(ASSEMBLE_FILES, 5, "assemble",
        "g++ ../src/config.c ../src/commands.c ../src/assemble.c -o assemble"))
    {
        printf("Make aborted!\n");
        return 0;
    }

    if (update(DISASSEMBLE_FILES, 7, "disassemble",
        "g++ ../src/config.c ../src/commands.c ../src/code_utils.c ../src/disassemble.c -o disassemble"))
    {
    printf("Make aborted!\n");
    return 0;
    }

    if (update(RUN_FILES, 9, "run",
        "g++ ../src/config.c ../src/commands.c ../src/code_utils.c ../../MyStack/stack.c ../src/run.c -o run"))
    {
    printf("Make aborted!\n");
    return 0;
    }

    if (arc >= 2)
    {
        char asm_command[50] = "./assemble < ";
        strcat(asm_command, argv[1]);
        strcat(asm_command, " > ");
        strcat(asm_command, BYTECODE_FILE);
        system(asm_command);

        char run_command[50] = "./run < ";
        strcat(run_command, BYTECODE_FILE);
        system(run_command);
    }

    return 0;
}
