#ifndef HASHMAP_H
#define HASHMAP_H

#define HASH_SIZE 100

typedef struct Node {
    char *key;
    void *value;
    struct Node *prev;
    struct Node *next;
} Node;

typedef struct {
    Node *table[HASH_SIZE];
} HashMap;

Node *new_node(const char *key, const void *value, int size);

void add_to_list(Node* root, Node* node);

Node* get_from_list(Node* root, const char* key);

void remove_from_list(Node** root, const char* key);

HashMap *create_map();

unsigned int hash(const char *key);

void insert(HashMap *map, const char *key, const void *value, int size);

Node* get_node(HashMap *map, const char *key);

void remove_from_map(HashMap *map, const char* key);

void free_map(HashMap *map);

#endif