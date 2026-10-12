#include "hash_table.h"


bool USE_FNV_HASH = true;

//private function declarations
static void _push(LinkedList *list, Node *new_node);

static void add_item_arr(dynamic_arr *arr, LinkedList *item, size_t index);

static void increase_capacity(dynamic_arr *arr);

static int get_item_arr(dynamic_arr *arr, size_t index, LinkedList **output);

static Node* _search_list(LinkedList *list, const char *key);

static void _delete_node(LinkedList *list, const char *key);

static int delete_item_arr(dynamic_arr *arr, size_t index);

static void free_dynamic_array(dynamic_arr * arr);

static void free_list_memory(LinkedList *list);

static void rehash(HashTable *hashtbl);

static double load_factor(HashTable *hashtbl);





//private function definitions

static double load_factor(HashTable *hashtbl){

    double load_fact = 0; 
    if (hashtbl->Buckets.capacity > 0 && hashtbl->Buckets.node_count > 0){
        load_fact = (double)hashtbl->Buckets.node_count / (double)hashtbl->Buckets.capacity;
    }

    return load_fact;

}

static unsigned int hash_func(char *input){

    unsigned int sum = 0;

    while (*input != '\0'){
        
        sum += *input;

        input++;

    }
    return sum;
}

uint64_t hash_FNV(const char *input){

    //init hash to FNV_OFFSET
    uint64_t hash = FNV_OFFSET;

    for (size_t i = 0; input[i] != '\0'; i++){

        //perform XOR operation
        hash = hash ^ (unsigned char)input[i];

        //multiply by FNV_PRIME
        hash = hash * FNV_PRIME;

    }

    return hash;

}


HashTable* _init_hash_tbl(size_t capacity){

    HashTable *tbl = calloc(1, sizeof(*tbl));
    if (tbl == NULL){
        exit(EXIT_FAILURE);
    }
    tbl->Buckets.capacity = capacity;
    tbl->Buckets.size = 0;
    tbl->Buckets.node_count = 0;
    tbl->Buckets.nodes = calloc(tbl->Buckets.capacity, sizeof *tbl->Buckets.nodes);
    if(tbl->Buckets.nodes == NULL){
        exit(EXIT_FAILURE);
    }
    return tbl;
}



//function definitions

static void rehash(HashTable *hashtbl){

    //make a new array to store rehashed nodes
    dynamic_arr temp_arr;
    temp_arr.capacity = hashtbl->Buckets.capacity;
    temp_arr.size = hashtbl->Buckets.size;
    temp_arr.nodes = calloc(temp_arr.capacity, sizeof *temp_arr.nodes);
    if (temp_arr.nodes == NULL){
        exit(EXIT_FAILURE);
    }

    //initialize new linkedlists into temp array and set head = NULL
    for (size_t i = 0; i < temp_arr.capacity; i++){

        LinkedList *list = calloc(1, sizeof(*list));
        if (list == NULL){
            exit(EXIT_FAILURE);
        }
        list->head = NULL;
        temp_arr.nodes[i] = list;
    }

    //rehash and sort nodes into temp array under correct indices
    for (size_t i = 0; i < hashtbl->Buckets.capacity; i++){
        //if Bucket is empty, continue
        if (hashtbl->Buckets.nodes[i] == NULL){
            continue;
        }

        Node *curr = hashtbl->Buckets.nodes[i]->head;
        Node *next_node = NULL;
        while (curr != NULL){

            //save link to next node
            next_node = curr->next;

            char *key = curr->key;
            size_t new_index;

            if (USE_FNV_HASH == true ){

                new_index = hash_FNV(key) % hashtbl->Buckets.capacity;
            }

            else{

                new_index = hash_func(key) % hashtbl->Buckets.capacity;
            }


            if (temp_arr.nodes[new_index]->head == NULL){

                temp_arr.nodes[new_index]->head = curr;

                curr->next = NULL;

            }

            else{

                curr->next = temp_arr.nodes[new_index]->head;

                temp_arr.nodes[new_index]->head = curr;
            }


            curr = next_node;

        }

    }


    //copy linkedlist heads from temp array back into original
    for (size_t i = 0; i < temp_arr.capacity; i++){


        if (hashtbl->Buckets.nodes[i] == NULL && temp_arr.nodes[i]->head != NULL){

            LinkedList *list = calloc(1, sizeof(*list));
            if (list == NULL){
                exit(EXIT_FAILURE);
            }

            list->head = NULL;

            hashtbl->Buckets.nodes[i] = list;

        }
        else if (hashtbl->Buckets.nodes[i] == NULL && temp_arr.nodes[i]->head == NULL){

            continue;
        }
        else if (hashtbl->Buckets.nodes[i] != NULL && temp_arr.nodes[i]->head == NULL){

            free(hashtbl->Buckets.nodes[i]);

            hashtbl->Buckets.nodes[i] = NULL;

            continue;

        }

        hashtbl->Buckets.nodes[i]->head = temp_arr.nodes[i]->head;



    }

    //recalculate arr size based on number of buckets with non-null heads

    size_t new_size = 0;
    for (size_t i = 0; i < hashtbl->Buckets.capacity; i++){

        if (hashtbl->Buckets.nodes[i] == NULL){
            continue;
        }

        if (hashtbl->Buckets.nodes[i]->head != NULL){
            new_size++;
        }
        
    }
    hashtbl->Buckets.size = new_size;

    //free the temporary linked list structures in temp arr
    for (size_t i = 0; i < temp_arr.capacity; i++){
        free(temp_arr.nodes[i]);
    }

    //free memory of temporary array
    free(temp_arr.nodes);

}



int add_item(HashTable *hashtbl, char *key, int val){


    if (load_factor(hashtbl) > MAX_LOAD_FACTOR){

        //printf("tbl->size: %zu, tbl->capacity: %zu\n", hashtbl->Buckets.size, hashtbl->Buckets.capacity);
        increase_capacity(&hashtbl->Buckets);
        //printf("tbl->size: %zu, tbl->capacity: %zu\n", hashtbl->Buckets.size, hashtbl->Buckets.capacity);
        //rehash(hashtbl);
        rehash(hashtbl);
    }
    int output;

    if (get_item(hashtbl, key, &output) == 0){

        printf("can't add item due to duplicate key\n");

        return 1;

    }

    size_t index;

    if (USE_FNV_HASH == true ){

        index = hash_FNV(key) % hashtbl->Buckets.capacity;
    }

    else{

        index = hash_func(key) % hashtbl->Buckets.capacity;
    }


    //allocate memory for new node and assign key/val pair
    Node *node = calloc(1, sizeof *node);
    if (node == NULL){
        exit(EXIT_FAILURE);
    }
    node->key = calloc(strlen(key) + 1, sizeof(char));
    if (node->key == NULL){
        exit(EXIT_FAILURE);
    }
    strcpy(node->key, key);
    node->value = val;

    if (hashtbl->Buckets.nodes[index] == NULL){
        
    //allocate memory for linkedlist and insert into dynamic array
        LinkedList *list = calloc(1, sizeof *list);
        if (list == NULL){
            exit(EXIT_FAILURE);
        }
        list->head = node;
        add_item_arr(&hashtbl->Buckets, list, index);
        hashtbl->Buckets.node_count++;
    }

    else {
    //insert new node into front of already existing linkedlist in dynamic array bucket
        _push(hashtbl->Buckets.nodes[index], node);
        hashtbl->Buckets.node_count++;
    }


    return 0;

}

int get_item(HashTable *tbl, char *key, int *output){



    size_t index;

    if (USE_FNV_HASH == true ){

        index = hash_FNV(key) % tbl->Buckets.capacity;
    }

    else{

        index = hash_func(key) % tbl->Buckets.capacity;
    }

    LinkedList *list = NULL;
    Node *node = NULL;

    int item_found = get_item_arr(&tbl->Buckets, index, &list);

    if (item_found == 1){
        return 1;
    }

    else{

        
        node = _search_list(list, key);

        if (node != NULL){
            *output = node->value;
        }
        else{
            //printf("item not found\n");
            return 1;
        }
        
    }

    return 0;

}


int update_item(HashTable *tbl, char *key, int new_value){


    size_t index;

    if (USE_FNV_HASH == true ){

        index = hash_FNV(key) % tbl->Buckets.capacity;
    }

    else{

        index = hash_func(key) % tbl->Buckets.capacity;
    }

    LinkedList *list = NULL;
    Node *node = NULL;

    int item_found = get_item_arr(&tbl->Buckets, index, &list);

    if (item_found == 1){
        //printf("could not update due to item not found\n");
        return 1;
    }

    else{

        
        node = _search_list(list, key);

        if (node != NULL){
            //printf("updating item value\n");
            node->value = new_value;
        }
        else{
            //printf("could not update due to item not found\n");
            return 1;
        }
        
    }

    return 0;

}


int delete_item(HashTable *tbl, char *key){


    size_t index;

    if (USE_FNV_HASH == true ){

        index = hash_FNV(key) % tbl->Buckets.capacity;
    }

    else{

        index = hash_func(key) % tbl->Buckets.capacity;
    }

    LinkedList *list = NULL;
    Node *node = NULL;

    int item_found = get_item_arr(&tbl->Buckets, index, &list);

    if (item_found == 1){
        //printf("could not delete item due to not found\n");
        return 1;
    }

    else{

        
        node = _search_list(list, key);

        if (node != NULL){
            //remove node from dynamic array and linked list
            if (node == list->head){
                _delete_node(list, key);
                //remove list from bucket if list is only one node
                if (list->head == NULL){
                    delete_item_arr(&tbl->Buckets, index);
                }
                tbl->Buckets.node_count--;
            }
            else{ 
                _delete_node(list, key);
                tbl->Buckets.node_count--;
            }
        }
        
    }

    return 0;

}

void free_hash_table(HashTable *tbl){

        LinkedList *list = NULL;
        dynamic_arr *arr = NULL;

        for(size_t i = 0; i < tbl->Buckets.capacity; i++){
            list = tbl->Buckets.nodes[i];
            if (list == NULL){
                continue;
            }
            free_list_memory(list);
        }

        arr = &tbl->Buckets;
        free_dynamic_array(arr);
        free(tbl);
        printf("hashtable memory successfully freed\n");

}


static void increase_capacity(dynamic_arr *arr){

    //double the array capacity
    //allocate additional memory to our array 
    //using our doubled capacity
    LinkedList **temp = realloc(arr->nodes, (arr->capacity * 2) * sizeof *temp);;
    if (temp == NULL){
        exit(EXIT_FAILURE);
    }

    size_t old_capacity = arr->capacity;

    if (temp != NULL){
        arr->nodes = temp;
        arr->capacity *= 2;

        //initialize new memory slots to NULL
        for (size_t i = old_capacity; i < arr->capacity; i++){
            arr->nodes[i] = NULL;

        }

    }
    else{
        printf("problem allocating new memory");
    }


}

// static void decrease_capacity(dynamic_arr *arr){

//     LinkedList **temp = realloc(arr->nodes, (arr->capacity / 2) * sizeof *arr->nodes);
//     if (temp == NULL){
//         exit(EXIT_FAILURE);
//     }

//     if (temp != NULL){

//         arr->nodes = temp;
//         arr->capacity /= 2;
//     }
//     else{
//         printf("problem reallocating memory to shrink array");

//     }

// }

static void add_item_arr(dynamic_arr *arr, LinkedList *item, size_t index){

    assert(index < arr->capacity);

    assert(arr->nodes[index] == NULL);
    
    arr->size++;

    arr->nodes[index] = item;

}

static int delete_item_arr(dynamic_arr *arr, size_t index){

    //ensure index is valid
    assert(index >= 0 && index < arr->capacity);

    //create reference to node in order to free memory
    LinkedList *list = arr->nodes[index];

    //no valid item found, so early return
    if (list == NULL){
        return 1;
    }
    //set value to NULL to allow a new pointer to be set at that specified index
    arr->nodes[index] = NULL;

    arr->size--;

    //free up memory of linkedlist node
    free(list);

    return 0;
}



static int get_item_arr(dynamic_arr *arr, size_t index, LinkedList **output){
    assert(index >= 0 && index < arr->capacity);
    *output = arr->nodes[index];
    if (*output == NULL){
        return 1;
    }

    return 0; 
}




static void free_dynamic_array(dynamic_arr *arr){

    for (size_t i = 0; i < arr->capacity; i++){
        if (arr->nodes[i] == NULL){
            continue;
        }
        free(arr->nodes[i]);
    }

    free(arr->nodes);
}



static void _push(LinkedList *list, Node *new_node){


    if (list->head == NULL){
        list->head = new_node;
        list->head->next = NULL;
    }

    else{
        new_node->next = list->head;
        list->head = new_node;
    }


}

static void _delete_node(LinkedList *list, const char *key){

    Node *curr = list->head;

    //delete head
    if (strcmp(curr->key, key) == 0){
        list->head = curr->next;
        free(curr->key);
        free(curr);
        return;
    }

    //delete middle or tail value
    while (curr->next != NULL){
        if (strcmp(curr->next->key, key) == 0){
            Node *temp = curr;
            curr = curr->next;
            temp->next = curr->next;
            free(curr->key);
            free(curr);
            return;
        }

        curr = curr->next;

    }


}

static Node* _search_list(LinkedList *list, const char *key){

    Node *curr = list->head;


    while (curr != NULL){
        if (strcmp(curr->key, key) == 0){
            return curr;
        }

        curr = curr->next;
    }

    return NULL;
}


static void free_list_memory(LinkedList *list){

    Node *curr = list->head;

    while (curr != NULL){

        Node *prev = curr;
        curr = curr->next;
        free(prev->key);
        free(prev);
    }

    list->head = NULL;
}





