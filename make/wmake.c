#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>

#define MAX_FILE_PATH_LENGTH 64
#define FILE_NUMBER 20

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
    const char BYTECODE_FILE[] = "../bin/program.bin";

    const char ASSEMBLE_FILES[FILE_NUMBER][MAX_FILE_PATH_LENGTH] =
    {
        "../src/config.h",
        "../src/commands.h",
		"../src/assemble/name_table.h",
		"../src/assemble/text.h",

        "../src/config.c",
        "../src/commands.c",
        "../src/assemble/assemble.c",
		"../src/assemble/name_table.c",
		"../src/assemble/text.c"
	};

    const char DISASSEMBLE_FILES[FILE_NUMBER][MAX_FILE_PATH_LENGTH] =
    {
        "../src/config.h",
        "../src/commands.h",
        "../src/code_utils.h",

        "../src/config.c",
        "../src/commands.c",
        "../src/code_utils.c",
        "../src/disassemble/disassemble.c"
    };

    const char RUN_FILES[FILE_NUMBER][MAX_FILE_PATH_LENGTH] =
    {
        "../src/config.h",
        "../src/commands.h",
        "../src/code_utils.h",
		"../../MyStack/src/stack_errors.h",
		"../../MyStack/src/prot/stack_protection.h",
		"../../MyStack/src/stack_utils.h",
		"../../MyStack/src/stack.h",

        "../src/config.c",
        "../src/commands.c",
        "../src/code_utils.c",
		"../../MyStack/src/stack_errors.c",
		"../../MyStack/src/prot/stack_protection.c",
		"../../MyStack/src/stack_utils.c",
        "../../MyStack/src/stack.c",
        "../src/run/run.c"
    };

    if (update(ASSEMBLE_FILES, 9, "../bin/assemble.exe",
        "g++ ../src/config.c ../src/commands.c ../src/assemble/name_table.c ../src/assemble/text.c ../src/assemble/assemble.c -o ../bin/assemble.exe"))
    {
        printf("Make aborted!\n");
        return 0;
    }

    if (update(DISASSEMBLE_FILES, 7, "../bin/disassemble.exe",
        "g++ ../src/config.c ../src/commands.c ../src/code_utils.c ../src/disassemble/disassemble.c -o ../bin/disassemble.exe"))
    {
    printf("Make aborted!\n");
    return 0;
    }

    if (update(RUN_FILES, 15, "../bin/run.exe",
        "g++ ../../MyStack/src/stack_errors.c ../../MyStack/src/prot/stack_protection.c ../../MyStack/src/stack_utils.c ../../MyStack/src/stack.c ../src/config.c ../src/commands.c ../src/code_utils.c ../src/run/run.c -o ../bin/run.exe"))
    {
    printf("Make aborted!\n");
    return 0;
    }

    if (arc >= 2)
    {
        char asm_command[50] = "..\\bin\\assemble.exe ";
        strcat(asm_command, argv[1]);
        strcat(asm_command, " ");
        strcat(asm_command, BYTECODE_FILE);
        system(asm_command);

        char run_command[50] = "..\\bin\\run.exe ";
        strcat(run_command, BYTECODE_FILE);
        system(run_command);
    }

    return 0;
}
