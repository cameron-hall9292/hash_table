
#define NUM_TEST_NODES 1000
#include "hash_table.h"

static int test_hash_tbl(HashTable * tbl);
static size_t measure_bucket_spread(HashTable *tbl);
static size_t max_list_length(HashTable *tbl);
void test_hash_func();

size_t count_nodes(HashTable *tbl){
    
    size_t node_count = 0;
    for (size_t i = 0; i < tbl->Buckets.capacity; i++){
        if (tbl->Buckets.nodes[i] != NULL){
            Node *curr = tbl->Buckets.nodes[i]->head; 
            while (curr != NULL){
                node_count++;
                curr = curr->next;
            }
        }
    }

    return node_count;
}

static size_t measure_bucket_spread(HashTable *tbl){

    size_t occupied_buckets = 0;
    for (size_t i = 0; i < tbl->Buckets.capacity; i++){
        if (tbl->Buckets.nodes[i] != NULL){
            occupied_buckets++;
        }
    }

    return occupied_buckets;

}

static size_t max_list_length(HashTable *tbl){
    size_t max = 0;
    for (size_t i = 0; i < tbl->Buckets.capacity; i++){
        if (tbl->Buckets.nodes[i] != NULL){
            Node *curr = tbl->Buckets.nodes[i]->head;
            size_t list_length = 0;
            while (curr != NULL){

                list_length++;
                curr = curr->next;
            }
            if (list_length > max){
                max = list_length;
            }
        }
    }
    return max;
}

static int test_hash_tbl(HashTable * tbl){

    int output;
    output = 0;
    char key[32];

    //add a bunch of items
    for(int i = 0; i < NUM_TEST_NODES; i++){

        snprintf(key, sizeof(key), "key_%d", i);
        assert(add_item(tbl, key, i) == 0);

    }

    //get the items back
    for(int i = 0; i < NUM_TEST_NODES; i++){

        snprintf(key, sizeof(key), "key_%d", i);
        assert(get_item(tbl, key, &output) == 0);
        assert(output == i);
    }


    printf("node_count: %zu\n", count_nodes(tbl));
    assert(count_nodes(tbl) == NUM_TEST_NODES);

    //display number of occupied buckets
    size_t occupied_buckets = measure_bucket_spread(tbl);
    printf("num occupied buckets: %zu\n", occupied_buckets);

    size_t max_list_len = max_list_length(tbl);
    printf("max list length: %zu\n", max_list_len);

    printf("final capacity: %zu\n", tbl->Buckets.capacity);


    assert(get_item(tbl, "key_20", &output) == 0);
    assert(output == 20);


    //update all nodes
    for(int i = 0; i < NUM_TEST_NODES; i++){

        snprintf(key, sizeof(key), "key_%d", i);
        assert(update_item(tbl, key, i * 2) == 0);
    }

    //confirm items successfully updated
    for(int i = 0; i < NUM_TEST_NODES; i++){

        snprintf(key, sizeof(key), "key_%d", i);
        assert(get_item(tbl, key, &output) == 0);
        assert(output == i * 2);
    }

    //delete all nodes
    for(int i = 0; i < NUM_TEST_NODES; i++){

        snprintf(key, sizeof(key), "key_%d", i);
        assert(delete_item(tbl, key) == 0);
    }

    //make sure you can't delete nodes that have already been deleted
    for(int i = 0; i < NUM_TEST_NODES; i++){

        snprintf(key, sizeof(key), "key_%d", i);
        assert(delete_item(tbl, key) == 1);
    }
    
    //ensure you can't retrieve delete items
    for(int i = 0; i < NUM_TEST_NODES; i++){

        snprintf(key, sizeof(key), "key_%d", i);
        assert(get_item(tbl, key, &output) == 1);
    }


    assert(count_nodes(tbl) == 0);


    //add items back
    for(int i = 0; i < NUM_TEST_NODES; i++){

        snprintf(key, sizeof(key), "key_%d", i);
        assert(add_item(tbl, key, i) == 0);

    }

    //delete even entries
    for(int i = 0; i < NUM_TEST_NODES; i++){

        if (i % 2 == 0){
            snprintf(key, sizeof(key), "key_%d", i);
            assert(delete_item(tbl, key) == 0);
        }

    }

    //ensure we have half of our nodes
    assert(count_nodes(tbl) == (size_t)NUM_TEST_NODES / 2);

    //get every other item back
    for(int i = 0; i < NUM_TEST_NODES; i++){
        //ensure even items cannot be retrieve post deletion
        if (i % 2 == 0){
            snprintf(key, sizeof(key), "key_%d", i);
            assert(get_item(tbl, key, &output) == 1);
        }
        //ensure odd items still exist in table
        else{
            snprintf(key, sizeof(key), "key_%d", i);
            assert(get_item(tbl, key, &output) == 0);
            assert(output = i);
        }
    }

    //delete all nodes for final cleanup
    for(int i = 0; i < NUM_TEST_NODES; i++){
        //delete the odd items
        if (i % 2 != 0){
            snprintf(key, sizeof(key), "key_%d", i);
            assert(delete_item(tbl, key) == 0);
        }

    }


    assert(count_nodes(tbl) == 0);


    return 0;

}



void test_hash_func(){

    uint64_t test_val = 0;

    const char *test_inputs[] = {
    "",
    "a",
    "b",
    "c",
    "hello",
    "foobar",
    "abc",
    "cba",
    "test",
    "HashTable",
    "123456",
    "Hello World"
};

    for (size_t i = 0; i < sizeof(test_inputs) / sizeof(*test_inputs); i++){

        const char *str = test_inputs[i];
        test_val = hash_FNV(str);
        printf("Hash: 0x%016" PRIx64 "\n", test_val);

    }
}

int main(void){

    HashTable *tbl = _init_hash_tbl(128);

    //run tests on hash table
    int fnv_hash = test_hash_tbl(tbl);

    if (fnv_hash == 0){
        printf("all tests passed\n");
    }


    // USE_FNV_HASH = false;
    // int basic_hash = test_hash_tbl(tbl);

    // if (basic_hash == 0){
    //     printf("all tests passed\n");
    // }





    //free hash table memory
    free_hash_table(tbl);






    return 0;
}