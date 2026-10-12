#ifndef HASH_TABLE_H
#define HASH_TABLE_H




#define MAX_LOAD_FACTOR 1.0
#define FNV_OFFSET 0xcbf29ce484222325ULL
#define FNV_PRIME  0x100000001b3ULL

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include<inttypes.h>
#include <assert.h>

extern bool USE_FNV_HASH;



typedef struct Node{
    char *key;
    int value;
    struct Node *next;

} Node;

typedef struct LinkedList{
    struct Node *head;
}LinkedList;

typedef struct {
    LinkedList **nodes;
    size_t size;
    size_t capacity;
    size_t node_count;
} dynamic_arr;


typedef struct HashTable{
    dynamic_arr Buckets;
}HashTable;



//Public function declaration

HashTable* _init_hash_tbl(size_t capacity);

int add_item(HashTable *hashtbl, char *key, int val);

int get_item(HashTable *tbl, char *key, int *output);

int delete_item(HashTable *tbl, char *key);

int update_item(HashTable *tbl, char *key, int new_value);

void free_hash_table(HashTable *tbl);

uint64_t hash_FNV(const char *input);







#endif
