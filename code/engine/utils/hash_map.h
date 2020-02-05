#pragma once

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

template <typename K, typename V>
struct hash_node
{
	K Key;
	V Value;
};

// NOTE(insolence): HashMap with open addressing
// TODO(insolence): Probably switch to separate chaining (https://github.com/jamesroutley/write-a-hash-table/tree/master/07-appendix)
template <typename K, typename V>
struct hash_map
{
	int BaseSize;
	int Size;
	int Count;
	hash_node<K, V> **Nodes;
};

static const int BASE_SIZE = 50;

template <typename K, typename V>
void DeleteHashMap(hash_map<K, V> *HashMap);

template <typename K, typename V>
hash_map<K, V>* CreateHashMap();

template <typename K, typename V>
void Insert(hash_map<K, V> *HashMap, K Key, V Value);

template <typename K, typename V>
V Get(hash_map<K, V> *HashMap, K Key);

template <typename K, typename V>
void Remove(hash_map<K, V> *HashMap, K Key);

template <typename K, typename V>
void ResizeUp(hash_map<K, V> *HashMap);

template <typename K, typename V>
void ResizeDown(hash_map<K, V> *HashMap);
