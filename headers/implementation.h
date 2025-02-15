#ifndef IMPLEMENTATION_H
#define IMPLEMENTATION_H

#include "structures.h"
#include "hashmap.h"

char* take_line_from_file(char string[MAX_STRING_SIZE], FILE* file);

void add_admin(HashMap* admin_map, char* username, char* password, char output[SOCKETBUFFERSIZE]);

void add_user(HashMap* user_map, char* username, char* password, char output[SOCKETBUFFERSIZE]);

void add_book(HashMap* books_map, char* id, char* author, char* name, int quantity_available, int quantity_borrowed, char output[SOCKETBUFFERSIZE]);

void borrow_book(HashMap* borrowed_books_map, HashMap* books_map, char* username, char* book_id, int max_books, char output[SOCKETBUFFERSIZE]);

void remove_admin(HashMap* admin_map, char* username, char output[SOCKETBUFFERSIZE]);

void remove_user(HashMap* user_map, HashMap* borrowed_books_map, char* username, char output[SOCKETBUFFERSIZE]);

void remove_book(HashMap* books_map, char* id, char output[SOCKETBUFFERSIZE]);

void return_book(HashMap* borrowed_books_map, HashMap* books_map, char* username, char* book_id, char output[SOCKETBUFFERSIZE]);

void edit_book(HashMap* books_map, char* id, char* attr_value, char* attr_to_change, char output[SOCKETBUFFERSIZE]);

void edit_admin_password(HashMap* admin_map, char* username, char* password, char output[SOCKETBUFFERSIZE]);

void edit_user_password(HashMap* user_map, char* username, char* password, char output[SOCKETBUFFERSIZE]);

void parse_borrowed_books(HashMap* borrowed_books_map, HashMap* books_map, char* username);

int load_from_file_admins(FILE* file, HashMap* admin_map, int* book_limit);

void dump_to_file_admins(FILE* file, HashMap* admin_map, int book_limit);

void load_from_file_users(FILE* file, HashMap* user_map);

void dump_to_file_users(FILE* file, HashMap* user_map);

void load_from_file_books(FILE* file, HashMap* books_map);

void dump_to_file_books(FILE* file, HashMap* books_map);

void load_from_file_borrowed_books(FILE* file, HashMap* books_map, HashMap* borrowed_books_map, int max_books);

void dump_to_file_borrowed_books(FILE* file, HashMap* borrowed_books_map);

void show_users(HashMap* user_map, char output[SOCKETBUFFERSIZE]);

void show_books(HashMap* books_map, char output[SOCKETBUFFERSIZE]);

void show_borrowed_books_of_user(HashMap* borrowed_books_map, char* username, char output[SOCKETBUFFERSIZE]);

void show_all_borrowed_books(HashMap* borrowed_books_map, HashMap* books_map, char output[SOCKETBUFFERSIZE]);

#endif