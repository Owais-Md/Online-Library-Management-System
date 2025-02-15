#ifndef STRUCTURES_H
#define STRUCTURES_H

#define MAX_STRING_SIZE 100
#define SOCKETBUFFERSIZE 1024

// Forward declaration of Node
typedef struct Node Node;

typedef struct admin{
    char username[MAX_STRING_SIZE];
    char password[MAX_STRING_SIZE];
}admin;

typedef struct user{
    char username[MAX_STRING_SIZE];
    char password[MAX_STRING_SIZE];
}user;

typedef struct books{
    char id[MAX_STRING_SIZE];
    char author[MAX_STRING_SIZE];
    char name[MAX_STRING_SIZE];
    int quantity_available;
    int quantity_borrowed;
}books;

typedef struct borrowed_books{
    char username[MAX_STRING_SIZE];
    Node* books_linked_list;
    int num;
}borrowed_books;

#endif