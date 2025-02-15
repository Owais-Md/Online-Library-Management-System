#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../headers/structures.h"
#include "../headers/hashmap.h"
#include "../headers/implementation.h"

char* take_line_from_file(char string[MAX_STRING_SIZE], FILE* file){
    if(fgets(string, MAX_STRING_SIZE, file) == NULL) return NULL;
    string[strcspn(string, "\n")] = '\0';
}

void add_admin(HashMap* admin_map, char* username, char* password, char output[SOCKETBUFFERSIZE]){
    memset(output, 0, SOCKETBUFFERSIZE);
    if (get_node(admin_map, username) != NULL){
        sprintf(output, "Error: Admin with username %s already exists\n", username);
        return;
    }
    insert(admin_map, username, password, MAX_STRING_SIZE);
}

void add_user(HashMap* user_map, char* username, char* password, char output[SOCKETBUFFERSIZE]){
    memset(output, 0, SOCKETBUFFERSIZE);
    //check if already exists
    if (get_node(user_map, username) != NULL){
        sprintf(output, "Error: User with username %s already exists\n", username);
        return;
    }
    insert(user_map, username, password, MAX_STRING_SIZE);
}

void add_book(HashMap* books_map, char* id, char* author, char* name, int quantity_available, int quantity_borrowed, char output[SOCKETBUFFERSIZE]){
    memset(output, 0, SOCKETBUFFERSIZE);
    if (get_node(books_map, id) != NULL){
        sprintf(output, "Error: Book with id %s already exists\n", id);
        return;
    }
    books* book = (books*) malloc(sizeof(books));
    strcpy(book->id, id);
    strcpy(book->author, author);
    strcpy(book->name, name);
    book->quantity_available = quantity_available;
    book->quantity_borrowed = quantity_borrowed;
    insert(books_map, id, book, sizeof(books));
}

void borrow_book(HashMap* borrowed_books_map, HashMap* books_map, char* username, char* book_id, int max_books, char output[SOCKETBUFFERSIZE]){
    memset(output, 0, SOCKETBUFFERSIZE);
    // checking if book exists
    Node* book_node = get_node(books_map, book_id);
    if (book_node == NULL) {
        sprintf(output, "Error: Book with id %s not found\n", book_id);
        return;
    }
    // as book exists, declaring book pointer to manipulate book
    books* book = book_node->value;
    if (book->quantity_available - book->quantity_borrowed == 0) {
        sprintf(output, "Error: Book with id %s not available\n", book_id);
        return;
    }
    // as book is available for borrow, finding borrowed_books_node
    Node* user_borrowed_books_node = get_node(borrowed_books_map, username);
    if(user_borrowed_books_node == NULL){
        // if not yet declared, creating new node, to put into map
        borrowed_books* borrowed_books_list = (borrowed_books*) malloc(sizeof(borrowed_books));
        strcpy(borrowed_books_list->username, username);
        borrowed_books_list->num = 1;
        borrowed_books_list->books_linked_list = (Node*) new_node(book_id, username, MAX_STRING_SIZE);
        // adding first admin, borrowed_books pair, which has books_linked_list in borrowed_books
        insert(borrowed_books_map, username, borrowed_books_list, sizeof(borrowed_books));
        book->quantity_borrowed++;
    }
    else{
        // if already declared, checking if book already borrowed
        borrowed_books* borrowed_books_list = user_borrowed_books_node->value;
        Node* book_node = get_from_list(borrowed_books_list->books_linked_list, book_id);
        if(book_node != NULL){
            sprintf(output, "Error: Book with id %s already borrowed by user %s\n", book_id, username);
            return;
        }
        // if cannot borrow any more books, returning
        if(borrowed_books_list->num >= max_books){
            sprintf(output, "Error: User %s has already borrowed %d books\n", username, max_books);
            return;
        }
        // if can borrow books, borrowing
        borrowed_books_list->num++;
        book->quantity_borrowed++;
        // adding to linked list, a new node with book
        add_to_list(borrowed_books_list->books_linked_list, new_node(book_id, username, MAX_STRING_SIZE));
    }
}


void remove_admin(HashMap* admin_map, char* username, char output[SOCKETBUFFERSIZE]){
    memset(output, 0, SOCKETBUFFERSIZE);
    Node* admin = get_node(admin_map, username);
    if (admin == NULL) {
        sprintf(output, "Error: Admin with username %s not found\n", username);
        return;
    }
    remove_from_map(admin_map, username);
}

void remove_user(HashMap* user_map, HashMap* borrowed_books_map, char* username, char output[SOCKETBUFFERSIZE]){
    memset(output, 0, SOCKETBUFFERSIZE);
    Node* user = get_node(user_map, username);
    if (user == NULL) {
        sprintf(output, "Error: User with username %s not found\n", username);
        return;
    }
    remove_from_map(user_map, username);
    // if user has borrowed books, do not let user get removed
    Node* borrowed_books_node = get_node(borrowed_books_map, username);
    if(borrowed_books_node != NULL){
        sprintf(output, "Error: User with username %s has borrowed books, cannot remove user\n", username);
        return;
    }
    remove_from_map(borrowed_books_map, username);
}

void remove_book(HashMap* books_map, char* id, char output[SOCKETBUFFERSIZE]){
    memset(output, 0, SOCKETBUFFERSIZE);
    Node* book = get_node(books_map, id);
    if (book == NULL) {
        sprintf(output, "Error: Book with id %s not found\n", id);
        return;
    }
    remove_from_map(books_map, id);
}

void return_book(HashMap* borrowed_books_map, HashMap* books_map, char* username, char* book_id, char output[SOCKETBUFFERSIZE]){
    memset(output, 0, SOCKETBUFFERSIZE);
    Node* book_node = get_node(books_map, book_id);
    if (book_node == NULL) {
        sprintf(output, "Error: Book with id %s not found\n", book_id);
        return;
    }
    books* book = book_node->value;
    Node* user_borrowed_books_node = get_node(borrowed_books_map, username);
    if(user_borrowed_books_node == NULL){
        sprintf(output, "Error: User with username %s never borrowed any book\n", username);
        return;
    }
    borrowed_books* borrowed_books_list = user_borrowed_books_node->value;
    Node* borrowed_book_node = get_from_list(borrowed_books_list->books_linked_list, book_id);
    if(borrowed_book_node == NULL){
        sprintf(output, "Error: Book with id %s was never borrowed by user %s\n", book_id, username);
        return;
    }
    remove_from_list(&borrowed_books_list->books_linked_list, book_id);
    book->quantity_borrowed--;
    borrowed_books_list->num--;
    if(borrowed_books_list->num == 0){
        remove_from_map(borrowed_books_map, username);
    }
}

void edit_book(HashMap* books_map, char* id, char* attr_value, char* attr_to_change, char output[SOCKETBUFFERSIZE]){
    memset(output, 0, SOCKETBUFFERSIZE);
    Node* book_node = get_node(books_map, id);
    if (book_node == NULL) {
        sprintf(output, "Error: Book with id %s not found\n", id);
        return;
    }
    books* book = book_node->value;
    if (strcmp(attr_to_change, "author") == 0) {
        strcpy(book->author, attr_value);
    }
    else if (strcmp(attr_to_change, "name") == 0) {
        strcpy(book->name, attr_value);
    }
    else if (strcmp(attr_to_change, "quantity_available") == 0) {
        book->quantity_available = atoi(attr_value);
    }
    else if (strcmp(attr_to_change, "quantity_borrowed") == 0) {
        book->quantity_borrowed = atoi(attr_value);
    }
    else {
        sprintf(output, "Error: Invalid attribute to change, you can only change author, name, quantity_available, or quantity_borrowed\n");
    }
}

// editing passwords
void edit_admin_password(HashMap* admin_map, char* username, char* password, char output[SOCKETBUFFERSIZE]){
    memset(output, 0, SOCKETBUFFERSIZE);
    Node* admin = get_node(admin_map, username);
    if (admin == NULL) {
        sprintf(output, "Error: Admin with username %s not found\n", username);
        return;
    }
    strcpy((char*) admin->value, password);
}

void edit_user_password(HashMap* user_map, char* username, char* password, char output[SOCKETBUFFERSIZE]){
    memset(output, 0, SOCKETBUFFERSIZE);
    Node* user = get_node(user_map, username);
    if (user == NULL) {
        sprintf(output, "Error: User with username %s not found\n", username);
        return;
    }
    strcpy((char*) user->value, password);
}

// to see if any books deleted by admin were borrowed by user, and return current number of borrowed books
void parse_borrowed_books(HashMap* borrowed_books_map, HashMap* books_map, char* username){
    Node* user_borrowed_books_node = get_node(borrowed_books_map, username);
    if(user_borrowed_books_node == NULL){
        return;
    }
    borrowed_books* borrowed_books_list = user_borrowed_books_node->value;
    Node* book_node = borrowed_books_list->books_linked_list;
    while (book_node != NULL) {
        Node* book = get_node(books_map, book_node->key);
        if(book == NULL){
            remove_from_list(&borrowed_books_list->books_linked_list, book_node->key);
            borrowed_books_list->num--;
        }
        book_node = book_node->next;
    }
    if(borrowed_books_list->num == 0){
        remove_from_map(borrowed_books_map, username);
    }
}

int load_from_file_admins(FILE* file, HashMap* admin_map, int* book_limit){
    int admin_count = 0;
    char username[MAX_STRING_SIZE];
    char password[MAX_STRING_SIZE];
    char temp[SOCKETBUFFERSIZE];
    rewind(file);
    if(fscanf(file, "%d", book_limit)!=EOF){
        take_line_from_file(password, file); // using password as a temp variable to consume newline
        while(take_line_from_file(username, file) != NULL){
            take_line_from_file(password, file);
            add_admin(admin_map, username, password, temp);
            admin_count++;
        }
    }
    return admin_count;    
}

void dump_to_file_admins(FILE* file, HashMap* admin_map, int book_limit){
    rewind(file);
    fprintf(file, "%d\n", book_limit);
    for(int i=0; i<HASH_SIZE; i++){
        Node* current = admin_map->table[i];
        while (current != NULL) {
            fprintf(file, "%s\n%s\n", current->key, (char*) current->value);
            current = current->next;
        }
    }
}

void load_from_file_users(FILE* file, HashMap* user_map){
    rewind(file);
    char username[MAX_STRING_SIZE];
    char password[MAX_STRING_SIZE];
    char temp[SOCKETBUFFERSIZE];
    while(take_line_from_file(username, file) != NULL){
        take_line_from_file(password, file);
        add_user(user_map, username, password, temp);
    }
}

void dump_to_file_users(FILE* file, HashMap* user_map){
    rewind(file);
    for(int i=0; i<HASH_SIZE; i++){
        Node* current = user_map->table[i];
        while (current != NULL) {
            fprintf(file, "%s\n%s\n", current->key, (char*) current->value);
            current = current->next;
        }
    }
}

void load_from_file_books(FILE* file, HashMap* books_map){
    rewind(file);
    char id[MAX_STRING_SIZE];
    char author[MAX_STRING_SIZE];
    char name[MAX_STRING_SIZE];
    char temp[SOCKETBUFFERSIZE];
    int quantity_available;
    int quantity_borrowed;
    while (take_line_from_file(id, file) != NULL) {
        take_line_from_file(author, file);
        take_line_from_file(name, file);
        fscanf(file, "%d %d", &quantity_available, &quantity_borrowed);
        take_line_from_file(temp, file); // using temp to read newline 
        add_book(books_map, id, author, name, quantity_available, quantity_borrowed, temp);
    }
}

void dump_to_file_books(FILE* file, HashMap* books_map){
    rewind(file);
    for(int i=0; i<HASH_SIZE; i++){
        Node* current = books_map->table[i];
        while (current != NULL) {
            books* book = (books*) current->value;
            fprintf(file, "%s\n%s\n%s\n%d %d\n", current->key, book->author, book->name, book->quantity_available, book->quantity_borrowed);
            current = current->next;
        }
    }
}

void load_from_file_borrowed_books(FILE* file, HashMap* books_map, HashMap* borrowed_books_map, int max_books){
    rewind(file);
    char username[MAX_STRING_SIZE], book_id[MAX_STRING_SIZE];
    int num;
    while (take_line_from_file(username, file) != NULL) {
        fscanf(file, "%d", &num);
        take_line_from_file(book_id, file); // using book_id as a temp variable to consume newline
        for (int i = 0; i < num; i++) {
            take_line_from_file(book_id, file);
            // borrow_book(borrowed_books_map, books_map, username, book_id, max_books);
            // cannot directly use borrow_book, as that will increment borrowed count, so have to add it manually
            Node* book_node = get_node(books_map, book_id);
            if (book_node == NULL) {
                return;
            }
            books* book = book_node->value;
            // book->quantity_borrowed++;
            Node* user_borrowed_books_node = get_node(borrowed_books_map, username);
            if(user_borrowed_books_node == NULL){
                borrowed_books* borrowed_books_list = (borrowed_books*) malloc(sizeof(borrowed_books));
                strcpy(borrowed_books_list->username, username);
                borrowed_books_list->num = 1;
                borrowed_books_list->books_linked_list = (Node*) new_node(book_id, username, MAX_STRING_SIZE);
                insert(borrowed_books_map, username, borrowed_books_list, sizeof(borrowed_books));
            }
            else{
                borrowed_books* borrowed_books_list = user_borrowed_books_node->value;
                borrowed_books_list->num++;
                add_to_list(borrowed_books_list->books_linked_list, new_node(book_id, username, MAX_STRING_SIZE));
            }
        }
    }
}

void dump_to_file_borrowed_books(FILE* file, HashMap* borrowed_books_map){
    rewind(file);
    for(int i=0; i<HASH_SIZE; i++){
        Node* current = borrowed_books_map->table[i];
        while (current != NULL) {
            borrowed_books* borrowed_books_list = (borrowed_books*) current->value;
            fprintf(file, "%s\n%d\n", current->key, borrowed_books_list->num);
            Node* book_node = borrowed_books_list->books_linked_list;
            int storedbookcount = 0;
            printf("storing list of books\n");
            while (book_node != NULL) {
                fprintf(file, "%s\n", (char*) book_node->key);
                book_node = book_node->next;
            }
            current = current->next;
        }
    }
}

void show_users(HashMap* user_map, char output[SOCKETBUFFERSIZE]){
    memset(output, 0, SOCKETBUFFERSIZE);
    char temp[MAX_STRING_SIZE*2];
    int user_count = 0;
    for(int i=0; i<HASH_SIZE; i++){
        Node* current = user_map->table[i];
        while (current != NULL) {
            // printf("%d. %s\n", ++user_count, current->key);
            sprintf(temp, "%d. %s\n", ++user_count, current->key);
            strcat(output, temp);
            current = current->next;
        }
    }
}

void show_books(HashMap* books_map, char output[SOCKETBUFFERSIZE]){
    memset(output, 0, SOCKETBUFFERSIZE);
    char temp[MAX_STRING_SIZE*4];
    int book_count = 0;
    for(int i=0; i<HASH_SIZE; i++){
        Node* current = books_map->table[i];
        while (current != NULL) {
            books* book = (books*) current->value;
            // printf("%d. %s %s \"%s\" %d\n", ++book_count, current->key, book->author, book->name, book->quantity_available - book->quantity_borrowed);
            sprintf(temp, "%d. %s %s \"%s\" %d\n", ++book_count, current->key, book->author, book->name, book->quantity_available - book->quantity_borrowed);
            strcat(output, temp);
            current = current->next;
        }
    }
}

void show_borrowed_books_of_user(HashMap* borrowed_books_map, char* username, char output[SOCKETBUFFERSIZE]){
    memset(output, 0, SOCKETBUFFERSIZE);
    char temp[MAX_STRING_SIZE*2];
    Node* user_borrowed_books_node = get_node(borrowed_books_map, username);
    if(user_borrowed_books_node == NULL){
        // fprintf(stderr, "Error: User with username %s never borrowed any book\n", username);
        sprintf(output, "Error: User with username %s never borrowed any book\n", username);
        return;
    }
    borrowed_books* borrowed_books_list = user_borrowed_books_node->value;
    Node* book_node = borrowed_books_list->books_linked_list;
    int borrowed_count = 0;
    while (book_node != NULL) {
        // printf("%d. %s\n", ++borrowed_count, (char*) book_node->key);
        sprintf(temp, "%d. %s\n", ++borrowed_count, (char*) book_node->key);
        strcat(output, temp);
        book_node = book_node->next;
    }
}

void show_all_borrowed_books(HashMap* borrowed_books_map, HashMap* books_map, char output[SOCKETBUFFERSIZE]){
    memset(output, 0, SOCKETBUFFERSIZE);
    char temp[MAX_STRING_SIZE*3];
    sprintf(output, "Printing all borrowed books\n");
    int borrowed_count = 0;
    for(int i=0; i<HASH_SIZE; i++){
        Node* current = borrowed_books_map->table[i];
        while (current != NULL) {
            borrowed_books* borrowed_books_list = (borrowed_books*) current->value;
            parse_borrowed_books(borrowed_books_map, books_map, borrowed_books_list->username);
            Node* book_node = borrowed_books_list->books_linked_list;
            while (book_node != NULL) {
                // printf("%d. %s %s\n", ++borrowed_count, (char*) borrowed_books_list->username, (char*) book_node->key);
                sprintf(temp, "%d. %s %s\n", ++borrowed_count, (char*) borrowed_books_list->username, (char*) book_node->key);
                strcat(output, temp);
                book_node = book_node->next;
            }
            current = current->next;
        }
    }
}