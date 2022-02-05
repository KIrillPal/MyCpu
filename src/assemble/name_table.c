#include "name_table.h"
#include <stdlib.h>
#include <stdio.h>

#define TRY(function) { NameTableStatus err = function; if (err != NT_OK) return err; }

static NameTableStatus NTNodeCtor(NTNode* node, char key)
{
	node->character = key;
	node->count = 0;
	node->nodes_size = 0;
	node->nodes_cap = 1;

	node->nodes = (NTNode*)calloc(node->nodes_cap, sizeof(NTNode));
	if (!node->nodes)
		return NT_MEM_ERROR;

	return NT_OK;
}

static NameTableStatus CreateNode(NTNode ** node, char key)
{
	*node = (NTNode*)calloc(1, sizeof(NTNode));

	if (*node == NULL)
		return NT_MEM_ERROR;

	return NTNodeCtor(*node, key);
}

static void NTNodeDtor(NTNode* node)
{
	for (int i = 0; i < node->nodes_size; ++i)
		NTNodeDtor(node->nodes + i);

	free(node->nodes);
}

static NameTableStatus InsertNode(NTNode* node, char key)
{
	if (node->nodes_cap == node->nodes_size)
	{
		node->nodes_cap *= 2;
		node->nodes = (NTNode*)realloc(node->nodes, node->nodes_cap * sizeof(NTNode));
		if (!node->nodes)
			return NT_MEM_ERROR;
	}

	return NTNodeCtor(node->nodes + node->nodes_size++, key);
}


static int NodeSetValue(NTNode* node, const char * str, ntval_t value)
{
	if (*str == '\0')
	{
		node->value = value;
		node->count++;
		return node->count;
	}

	for (int i = 0; i < node->nodes_size; ++i)
	{
		if (*str == (node->nodes + i)->character)
			return NodeSetValue(node->nodes + i, str + 1, value);
	}
	TRY(InsertNode(node, *str));
	return NodeSetValue(node->nodes + (node->nodes_size - 1), str + 1, value);
}

static int NodeGetValue(NTNode* node, const char * str, ntval_t * value)
{
	if (*str == '\0')
	{
		*value = node->value;
		return node->count;
	}

	for (int i = 0; i < node->nodes_size; ++i)
	{
		if (*str == (node->nodes + i)->character)
			return NodeGetValue(node->nodes + i, str + 1, value);
	}

	return 0;
}

NameTableStatus NameTableCtor(NameTable * table)
{
	table->size = 0;

	NTNode * root;
	TRY(CreateNode(&root, 0));

	table->root = root;

	return NT_OK;
}

NameTableStatus NameTableDtor(NameTable* table)
{
	NTNodeDtor(table->root);
	free(table->root);

	table->root = NULL;
	table->size = 0;

	return NT_OK;
}

int NameTableSetValue(NameTable* table, const char * str, ntval_t value)
{
	return NodeSetValue(table->root, str, value);
}

int NameTableGetValue(NameTable* table, const char* str, ntval_t * value)
{
	return NodeGetValue(table->root, str, value);
}