
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
} dynamic_arr;


typedef struct HashTable{
    dynamic_arr Buckets;
}HashTable;



//Public function declaration

HashTable* _init_hash_tbl();

int add_item(HashTable *hashtbl, char *key, int val);

int get_item(HashTable *tbl, char *key, int *output);

int delete_item(HashTable *tbl, char *key);

int update_item(HashTable *tbl, char *key, int new_value);


void free_hash_table(HashTable *tbl);

