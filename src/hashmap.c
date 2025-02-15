#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../headers/hashmap.h"

Node *new_node(const char *key, const void *value, int size){
    Node *node = (Node *)malloc(sizeof(Node));
    node->key = (char *)malloc(strlen(key) + 1);
    strcpy(node->key, key);
    node->value = malloc(size);
    memcpy(node->value, value, size);
    node->next = NULL;
    node->prev = NULL;
    return node;
}

// function to add to a linked_list
void add_to_list(Node* root, Node* node){
    Node* current = root;
    if(current == NULL){
        printf("adding to a null root list\n");
        // allocating memory to root
        current = (Node *)malloc(sizeof(Node));
        memcpy(current, node, sizeof(Node));
        printf("added to a null root list\n");
    }
    while (current->next != NULL){
        current = current->next;
    }
    current->next = node;
    node->prev = current;
}

// function to get from a linked_list
Node* get_from_list(Node* root, const char* key){
    Node* current = root;
    while (current != NULL){
        if (strcmp(current->key, key) == 0){
            return current;
        }
        current = current->next;
    }
    return NULL;
}

// function to remove from a linked_list
void remove_from_list(Node** root, const char* key){
    Node* current = *root;
    Node* prev = NULL;
    while (current != NULL){
        if (strcmp(current->key, key) == 0){
            if (prev == NULL){
                // removing root node
                *root = current->next;
                if (current->next != NULL){
                    current->next->prev = NULL;
                }
            }
            else{
                prev->next = current->next;
                if (current->next != NULL){
                    current->next->prev = prev;
                }
            }
            free(current->value);
            free(current);
            return; // Exit after removing the node.
        }
        prev = current;
        current = current->next;
    }
}


HashMap *create_map(){
    HashMap *map = (HashMap *)malloc(sizeof(HashMap));
    for (int i = 0; i < HASH_SIZE; i++){
        map->table[i] = NULL;
    }
    return map;
}

unsigned int hash(const char *key){
    unsigned int hashval = 0;
    while (*key){
        hashval = (hashval << 5) + *key++;
    }
    return hashval % HASH_SIZE;
}

void insert(HashMap *map, const char *key, const void *value, int size){
    unsigned int index = hash(key);
    Node *new = new_node(key, value, size);
    Node *current = map->table[index];
    if (current == NULL){
        map->table[index] = new;
    }
    else{
        add_to_list(current, new);
    }
}

// Function to retrieve value given a key
Node* get_node(HashMap *map, const char *key){
    unsigned int index = hash(key);
    Node *pair = map->table[index];
    
    return get_from_list(pair, key);
}

// Function to remove a node from a list
void remove_from_map(HashMap *map, const char* key){
    unsigned int index = hash(key);
    Node *pair = map->table[index];
    if (pair != NULL) {
        remove_from_list(&(map->table[index]), key);
    }
}


// Function to free memory allocated for hashmap
void free_map(HashMap *map){
    for (int i = 0; i < HASH_SIZE; i++){
        Node *pair = map->table[i];
        while (pair != NULL){
            Node *temp = pair;
            pair = pair->next;
            free(temp->key);
            free(temp->value);
            free(temp);
        }
    }
    free(map);
}