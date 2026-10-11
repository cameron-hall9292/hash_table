
# Hash Table in C!

This is a basic hash-table I have put together in an effort to learn the basics of programming in C. 


# Basic Architecture

My hash table is a dynamic array of buckets that contain linked-list heads. Right now, the hash function itself is ultra basic,
where it simply sums up the value of characters in a string. I started this way to ensure I understood exactly what a hash function is
supposed to do, which is to take a character string as an input and output a unique number that can be used to assign an index
to that particular string, the idea being that the same string input will output the same number and in turn, the same index, which is how you get O(1) lookups in a hash-table. With that said, I plan to improve the hash function by implementing a much more practical one that will reduce collisions.


# Usage

To use this hash-table, you want to clone the repo and utilize the following functions: 

```c

HashTable* _init_hash_tbl();

int add_item(HashTable *hashtbl, char *key, int val);

int get_item(HashTable *tbl, char *key, int *output);

int delete_item(HashTable *tbl, char *key);

int update_item(HashTable *tbl, char *key, int new_value);

void free_hash_table(HashTable *tbl);

```

It probably goes without saying, but I'll say it anyway, this hash-table implementation is not production quality,
meaning it doesn't handle memory allocation failures gracefully. Rather, I decided to terminate the program if memory
ever fails to allocate properly. However, if you would like to implement it in a small hobby project, then you should be
fine. If you do use it, be sure to free the memory once you're done using the hash-table.