#pragma once

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

struct hash_node
{
	char *Key;
	int Value;
};

// NOTE(insolence): HashMap with open addressing
// TODO(insolence): Probably switch to separate chaining (https://github.com/jamesroutley/write-a-hash-table/tree/master/07-appendix)
struct hash_map
{
	int BaseSize;
	int Size;
	int Count;
	hash_node **Nodes;
};

static hash_node REMOVED_NODE = { NULL, -1 };

static const int BASE_SIZE = 50;


void DeleteHashMap(hash_map *HashMap);
hash_map* CreateHashMap();
void Insert(hash_map *HashMap, const char *Key, int Value);
int  Get(hash_map *HashMap, const char *Key);
void Remove(hash_map *HashMap, const char *Key);
static void ResizeUp(hash_map *HashMap);
static void ResizeDown(hash_map *HashMap);
