#pragma once
#include <stddef.h>

typedef unsigned long long ntval_t;

enum NameTableStatus
{
	NT_OK = 0,
	NT_MEM_ERROR = -1
};

struct NTNode
{
	char character  = 0;
	char count      = 0;
	int nodes_size  = 0;
	int nodes_cap   = 0;
	ntval_t  value  = 0;
	NTNode * nodes  = NULL;
};

struct NameTable
{
	int size = 0;
	NTNode * root = NULL;
};

NameTableStatus NameTableCtor(NameTable* table);
NameTableStatus NameTableDtor(NameTable* table);

int NameTableSetValue(NameTable* table, const char* str, ntval_t value);
int NameTableGetValue(NameTable* table, const char* str, ntval_t* value);