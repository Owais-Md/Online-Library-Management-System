#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include "../headers/implementation.h"

#define PORT 8080

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

FILE* admin_file;
FILE* user_file;
FILE* books_file;
FILE* borrowed_books_file;

HashMap* admin_map;
HashMap* user_map;
HashMap* books_map;
HashMap* borrowed_books_map;

char output[SOCKETBUFFERSIZE];

int admin_count;
int book_limit = 0;

void receive_response(char response[MAX_STRING_SIZE], int socket){
    memset(response, 0, MAX_STRING_SIZE);
    while(1){
        if(recv(socket, response, MAX_STRING_SIZE, 0) > 0) break;
        else usleep(10);
    }
    printf("Received from socket %d, message: %s\n", socket, response);
}

void send_prompt(char prompt[SOCKETBUFFERSIZE], int socket, char* prompt_or_output){
    char recieve_confirmation[MAX_STRING_SIZE];
    send(socket, prompt_or_output, 10, 0);
    receive_response(recieve_confirmation, socket);
    send(socket, prompt, SOCKETBUFFERSIZE, 0);
}

void initialize_maps(){
    admin_map = create_map();
    if (admin_map == NULL) {
        fprintf(stderr, "Error initializing admin_map\n");
        exit(EXIT_FAILURE);
    }

    user_map = create_map();
    if (user_map == NULL) {
        fprintf(stderr, "Error initializing user_map\n");
        exit(EXIT_FAILURE);
    }

    books_map = create_map();
    if (books_map == NULL) {
        fprintf(stderr, "Error initializing books_map\n");
        exit(EXIT_FAILURE);
    }

    borrowed_books_map = create_map();
    if (borrowed_books_map == NULL) {
        fprintf(stderr, "Error initializing borrowed_books_map\n");
        exit(EXIT_FAILURE);
    }
    printf("Maps initialized successfully\n");
}


int adminLogin(char username[MAX_STRING_SIZE], char password[MAX_STRING_SIZE], int socket){
    char response[SOCKETBUFFERSIZE];
    char prompt[SOCKETBUFFERSIZE];
    strcpy(prompt, "Enter username: ");
    send_prompt(prompt, socket, "prompt");
    receive_response(response, socket);
    strcpy(username, response);
    strcpy(prompt, "Enter password: ");
    send_prompt(prompt, socket, "prompt");
    receive_response(response, socket);
    strcpy(password, response);
    pthread_mutex_lock(&mutex);
    Node* admin = get_node(admin_map, username);
    pthread_mutex_unlock(&mutex);
    if (admin == NULL) {
        strcpy(prompt, "Admin with username not found\n");
        send_prompt(prompt, socket, "output");
        return 0;
    } else if (strcmp((char*) admin->value, password) != 0) {
        strcpy(prompt, "Invalid password\n");
        send_prompt(prompt, socket, "output");
        return 0;
    }
    return 1;
}

void load_data(){
    printf("Opening all files needed to load data...\n");
    admin_file = fopen("./data/admin_file.txt", "r+");
    if (admin_file == NULL) {
        admin_file = fopen("./data/admin_file.txt", "w+");
        if (admin_file == NULL) {
            fprintf(stderr, "Error opening admin_file\n");
            exit(EXIT_FAILURE);
        }
    
    }
    user_file = fopen("./data/user_file.txt", "r+");
    if (user_file == NULL) {
        user_file = fopen("./data/user_file.txt", "w+");
        if (user_file == NULL) {
            fprintf(stderr, "Error opening user_file\n");
            exit(EXIT_FAILURE);
        }
    }
    books_file = fopen("./data/books_file.txt", "r+");
    if (books_file == NULL) {
        books_file = fopen("./data/books_file.txt", "w+");
        if (books_file == NULL) {
            fprintf(stderr, "Error opening books_file\n");
            exit(EXIT_FAILURE);
        }
    }
    borrowed_books_file = fopen("./data/borrowed_books_file.txt", "r+");
    if (borrowed_books_file == NULL) {
        borrowed_books_file = fopen("./data/borrowed_books_file.txt", "w+");
        if (borrowed_books_file == NULL) {
            fprintf(stderr, "Error opening borrowed_books_file\n");
            exit(EXIT_FAILURE);
        }
    }
    printf("Opened all files successfully\n");
    printf("Loading data...\n");
    pthread_mutex_lock(&mutex);
    admin_count = load_from_file_admins(admin_file, admin_map, &book_limit);
    pthread_mutex_unlock(&mutex);
    load_from_file_users(user_file, user_map);
    load_from_file_books(books_file, books_map);
    load_from_file_borrowed_books(borrowed_books_file, books_map, borrowed_books_map, book_limit);
    printf("Data loaded successfully\n");
    printf("Closing all opened files...\n");
    fclose(admin_file);
    fclose(user_file);
    fclose(books_file);
    fclose(borrowed_books_file);
}

void dump_data(){
    printf("Opening all files needed to dump data...\n");
    admin_file = fopen("./data/admin_file.txt", "w+");
    user_file = fopen("./data/user_file.txt", "w+");
    books_file = fopen("./data/books_file.txt", "w+");
    borrowed_books_file = fopen("./data/borrowed_books_file.txt", "w+");
    printf("Opened all files successfully\n");
    printf("Dumping data...\n");
    dump_to_file_admins(admin_file, admin_map, book_limit);
    dump_to_file_users(user_file, user_map);
    dump_to_file_books(books_file, books_map);
    dump_to_file_borrowed_books(borrowed_books_file, borrowed_books_map);
    printf("Data dumped successfully\n");
    printf("Closing all opened files...\n");
    fclose(admin_file);
    fclose(user_file);
    fclose(books_file);
    fclose(borrowed_books_file);
}

int userLogin(char username[MAX_STRING_SIZE], char password[MAX_STRING_SIZE], int socket){
    char response[SOCKETBUFFERSIZE];
    char prompt[SOCKETBUFFERSIZE];
    strcpy(prompt, "Enter username: ");
    send_prompt(prompt, socket, "prompt");
    receive_response(response, socket);
    strcpy(username, response);
    strcpy(prompt, "Enter password: ");
    send_prompt(prompt, socket, "prompt");
    receive_response(response, socket);
    strcpy(password, response);
    pthread_mutex_lock(&mutex);
    Node* user = get_node(user_map, username);
    pthread_mutex_unlock(&mutex);
    if (user == NULL) {
        strcpy(prompt, "User with username not found\n");
        send_prompt(prompt, socket, "output");
        return 0;
    } else if (strcmp((char*) user->value, password) != 0) {
        strcpy(prompt, "Invalid password\n");
        send_prompt(prompt, socket, "output");
        return 0;
    }
    return 1;
}

void adminMenu(char username[MAX_STRING_SIZE], char password[MAX_STRING_SIZE], int socket){
    int choice;
    char tempstr[5][MAX_STRING_SIZE];
    int tempint[2];
    char response[SOCKETBUFFERSIZE];
    char prompt[SOCKETBUFFERSIZE];
    while(1){
        strcpy(prompt, "Admin menu:\n1. Add book\n2. Remove book\n3. Edit book\n4. Add user\n5. Remove user\n6. Edit user\n7. Add admin\n8. Remove admin\n9. Edit admin\n10. Show all users\n11. Show all books\n12. Show all books borrowed by a particular user\n13. Show all books borrowed by every user\n14. Dump data\n15. Exit\n");
        send_prompt(prompt, socket, "prompt");
        receive_response(response, socket);
        choice = atoi(response);
        switch (choice) {
            case 1:
                strcpy(prompt, "Enter book id: ");
                send_prompt(prompt, socket, "prompt");
                receive_response(response, socket);
                strcpy(tempstr[0], response);
                strcpy(prompt, "Enter author: ");
                send_prompt(prompt, socket, "prompt");
                receive_response(response, socket);
                strcpy(tempstr[1], response);
                strcpy(prompt, "Enter name: ");
                send_prompt(prompt, socket, "prompt");
                receive_response(response, socket);
                strcpy(tempstr[2], response);
                strcpy(prompt, "Enter quantity available: ");
                send_prompt(prompt, socket, "prompt");
                receive_response(response, socket);
                tempint[0] = atoi(response);
                strcpy(prompt, "Enter quantity borrowed: ");
                send_prompt(prompt, socket, "prompt");
                receive_response(response, socket);
                tempint[1] = atoi(response);
                pthread_mutex_lock(&mutex);
                add_book(books_map, tempstr[0], tempstr[1], tempstr[2], tempint[0], tempint[1], output);
                pthread_mutex_unlock(&mutex);
                send_prompt(output, socket, "output");
                break;
            case 2:
                strcpy(prompt, "Enter book id: ");
                send_prompt(prompt, socket, "prompt");
                receive_response(response, socket);
                strcpy(tempstr[0], response);
                pthread_mutex_lock(&mutex);
                remove_book(books_map, tempstr[0], output);
                pthread_mutex_unlock(&mutex);
                send_prompt(output, socket, "output");
                break;
            case 3:
                strcpy(prompt, "Enter book id: ");
                send_prompt(prompt, socket, "prompt");
                receive_response(response, socket);
                strcpy(tempstr[0], response);
                strcpy(prompt, "Enter attribute to change: ");
                send_prompt(prompt, socket, "prompt");
                receive_response(response, socket);
                strcpy(tempstr[1], response);
                strcpy(prompt, "Enter new value of attribute for book with id: ");
                send_prompt(prompt, socket, "prompt");
                receive_response(response, socket);
                strcpy(tempstr[2], response);
                pthread_mutex_lock(&mutex);
                edit_book(books_map, tempstr[0], tempstr[2], tempstr[1], output);
                pthread_mutex_unlock(&mutex);
                send_prompt(output, socket, "output");
                break;
            case 4:
                strcpy(prompt, "Enter user id: ");
                send_prompt(prompt, socket, "prompt");
                receive_response(response, socket);
                strcpy(tempstr[0], response);
                strcpy(prompt, "Enter password: ");
                send_prompt(prompt, socket, "prompt");
                receive_response(response, socket);
                strcpy(tempstr[1], response);
                pthread_mutex_lock(&mutex);
                add_user(user_map, tempstr[0], tempstr[1], output);
                pthread_mutex_unlock(&mutex);
                send_prompt(output, socket, "output");
                break;
            case 5:
                strcpy(prompt, "Enter user id: ");
                send_prompt(prompt, socket, "prompt");
                receive_response(response, socket);
                strcpy(tempstr[0], response);
                pthread_mutex_lock(&mutex);
                remove_user(user_map, borrowed_books_map, tempstr[0], output);
                pthread_mutex_unlock(&mutex);
                send_prompt(output, socket, "output");
                break;
            case 6:
                strcpy(prompt, "Enter user id: ");
                send_prompt(prompt, socket, "prompt");
                receive_response(response, socket);
                strcpy(tempstr[0], response);
                strcpy(prompt, "Enter new password for user: ");
                send_prompt(prompt, socket, "prompt");
                receive_response(response, socket);
                strcpy(tempstr[1], response);
                pthread_mutex_lock(&mutex);
                edit_user_password(user_map, tempstr[0], tempstr[1], output);
                pthread_mutex_unlock(&mutex);
                send_prompt(output, socket, "output");
                break;
            case 7:
                strcpy(prompt, "Enter admin id: ");
                send_prompt(prompt, socket, "prompt");
                receive_response(response, socket);
                strcpy(tempstr[0], response);
                strcpy(prompt, "Enter password: ");
                send_prompt(prompt, socket, "prompt");
                receive_response(response, socket);
                strcpy(tempstr[1], response);
                pthread_mutex_lock(&mutex);
                add_admin(admin_map, tempstr[0], tempstr[1], output);
                pthread_mutex_unlock(&mutex);
                send_prompt(output, socket, "output");
                admin_count++;
                break;
            case 8:
                strcpy(prompt, "Enter admin id: ");
                send_prompt(prompt, socket, "prompt");
                receive_response(response, socket);
                strcpy(tempstr[0], response);
                pthread_mutex_lock(&mutex);
                remove_admin(admin_map, tempstr[0], output);
                pthread_mutex_unlock(&mutex);
                send_prompt(output, socket, "output");
                break;
            case 9:
                strcpy(prompt, "Enter new password for admin: ");
                send_prompt(prompt, socket, "prompt");
                receive_response(response, socket);
                strcpy(tempstr[0], response);
                pthread_mutex_lock(&mutex);
                edit_admin_password(admin_map, username, tempstr[0], output);
                pthread_mutex_unlock(&mutex);
                send_prompt(output, socket, "output");
                break;
            case 10:
                pthread_mutex_lock(&mutex);
                show_users(user_map, output);
                pthread_mutex_unlock(&mutex);
                send_prompt(output, socket, "output");
                break;
            case 11:
                pthread_mutex_lock(&mutex);
                show_books(books_map, output);
                pthread_mutex_unlock(&mutex);
                send_prompt(output, socket, "output");
                break;
            case 12:
                strcpy(prompt, "Enter user id: ");
                send_prompt(prompt, socket, "prompt");
                receive_response(response, socket);
                strcpy(tempstr[0], response);
                pthread_mutex_lock(&mutex);
                show_borrowed_books_of_user(borrowed_books_map, tempstr[0], output);
                pthread_mutex_unlock(&mutex);
                send_prompt(output, socket, "output");
                break;
            case 13:
                pthread_mutex_lock(&mutex);
                show_all_borrowed_books(borrowed_books_map, books_map, output);
                pthread_mutex_unlock(&mutex);
                send_prompt(output, socket, "output");
                break;
            case 14:
                strcpy(prompt, "Dumping all data into files\n");
                send_prompt(prompt, socket, "output");
                pthread_mutex_lock(&mutex);
                dump_data();
                pthread_mutex_unlock(&mutex);
                break;
            case 15:
                strcpy(prompt, "Exiting admin menu\n");
                send_prompt(prompt, socket, "output");
                return;
            default:
                strcpy(prompt, "Invalid choice. Please try again.\n");
                send_prompt(prompt, socket, "output");
                break;
        }
    }
}

void userMenu(char username[MAX_STRING_SIZE], char password[MAX_STRING_SIZE], int socket){
    int choice;
    char tempstr[5][MAX_STRING_SIZE];
    int tempint[1];
    char prompt[SOCKETBUFFERSIZE];
    char response[SOCKETBUFFERSIZE];
    while(1){
        strcpy(prompt, "User menu:\n1. Borrow book\n2. Return book\n3. Show all books\n4. Show all books I borrowed\n5. Change my password\n6. Exit\n");
        send_prompt(prompt, socket, "prompt");
        receive_response(response, socket);
        choice = atoi(response);
        switch (choice) {
            case 1:
                strcpy(prompt, "Enter book id: ");
                send_prompt(prompt, socket, "prompt");
                receive_response(response, socket);
                strcpy(tempstr[0], response);
                pthread_mutex_lock(&mutex);
                borrow_book(borrowed_books_map, books_map, username, tempstr[0], book_limit, output);
                pthread_mutex_unlock(&mutex);
                send_prompt(output, socket, "output");
                break;
            case 2:
                strcpy(prompt, "Enter book id: ");
                send_prompt(prompt, socket, "prompt");
                receive_response(response, socket);
                strcpy(tempstr[0], response);
                pthread_mutex_lock(&mutex);
                return_book(borrowed_books_map, books_map, username, tempstr[0], output);
                pthread_mutex_unlock(&mutex);
                send_prompt(output, socket, "output");
                break;
            case 3:
                pthread_mutex_lock(&mutex);
                show_books(books_map, output);
                pthread_mutex_unlock(&mutex);
                send_prompt(output, socket, "output");
                break;
            case 4:
                pthread_mutex_lock(&mutex);
                show_borrowed_books_of_user(borrowed_books_map, username, output);
                pthread_mutex_unlock(&mutex);
                send_prompt(output, socket, "output");
                break;
            case 5:
                strcpy(prompt, "Enter new password: ");
                send_prompt(prompt, socket, "prompt");
                receive_response(response, socket);
                strcpy(tempstr[0], response);
                pthread_mutex_lock(&mutex);
                edit_user_password(user_map, username, tempstr[0], output);
                pthread_mutex_unlock(&mutex);
                send_prompt(output, socket, "output");
                break;
            case 6:
                strcpy(prompt, "Exiting user menu\n");
                send_prompt(prompt, socket, "output");
                return;
            default:
                strcpy(prompt, "Invalid choice. Please try again.\n");
                send_prompt(prompt, socket, "output");
                break;
        }
    }
}

void ClientFunctions(int socket){
    printf("In clientFunctions()\n");
    char prompt[SOCKETBUFFERSIZE];
    char choice[MAX_STRING_SIZE];
    char username[MAX_STRING_SIZE], password[MAX_STRING_SIZE];
    int c;
    pthread_mutex_lock(&mutex);
    if(admin_count == 0){
        strcpy(prompt, "No admin accounts found\nUse default login:\nEnter username: ");
        send_prompt(prompt, socket, "prompt");
        receive_response(username, socket);
        strcpy(prompt, "Enter password: ");
        send_prompt(prompt, socket, "prompt");
        receive_response(password, socket);
        if(strcmp(username, "admin") == 0 && strcmp(password, "admin") == 0){
            strcpy(prompt, "Enter new admin username: ");
            send_prompt(prompt, socket, "prompt");
            receive_response(username, socket);
            strcpy(prompt, "Enter new admin password: ");
            send_prompt(prompt, socket, "prompt");
            receive_response(password, socket);
            strcpy(prompt, "Set book limit: ");
            send_prompt(prompt, socket, "prompt");
            while(book_limit <= 0){
                receive_response(choice, socket);
                book_limit = atoi(choice);
                if(book_limit <= 0){
                    strcpy(prompt, "Invalid book limit\n");
                    send_prompt(prompt, socket, "output");
                }
            }
            add_admin(admin_map, username, password, output);
            send_prompt(output, socket, "output");
            admin_count++;
            strcpy(prompt, "New admin added\n");
            send_prompt(prompt, socket, "output");
            pthread_mutex_unlock(&mutex);
        } else {
            strcpy(prompt, "Invalid login\n");
            send_prompt(prompt, socket, "output");
            strcpy(prompt, "Exiting program\n");            
            send_prompt(prompt, socket, "output");
            pthread_mutex_unlock(&mutex);
            return;
        }
    } 
    pthread_mutex_unlock(&mutex);
    while(1){
        strcpy(prompt, "Enter 1 for admin login, 2 for user login, 3 to quit: ");
        send_prompt(prompt, socket, "prompt");
        receive_response(choice, socket);
        c = atoi(choice);
        if(c == 1){
            if (adminLogin(username, password, socket)) {
                strcpy(prompt, "logged in as admin: ");
                send_prompt(prompt, socket, "output");
                adminMenu(username, password, socket);
            } else {
                strcpy(prompt, "Admin login failed\n");
                send_prompt(prompt, socket, "output");
            }
        } else if(c == 2){
            if (userLogin(username, password, socket)) {
                strcpy(prompt, "logged in as user: ");
                send_prompt(prompt, socket, "output");
                userMenu(username, password, socket);
            } else {
                strcpy(prompt, "User login failed\n");
                send_prompt(prompt, socket, "output");
            }
        } else if(c == 3){
            strcpy(prompt, "Exiting program\n");
            send_prompt(prompt, socket, "output");
            pthread_mutex_lock(&mutex);
            dump_data();
            pthread_mutex_unlock(&mutex);
            return;
        } else {
            strcpy(prompt, "Invalid choice. Please try again.\n");
            send_prompt(prompt, socket, "output");
        }
    }
}

void *clientHandler(void *arg) {
    int client_sock = *((int *)arg);
    free(arg); // free memory allocated for argument

    ClientFunctions(client_sock); // call ClientFunctions with the socket descriptor

    return NULL;
}

int main(){
    printf("Program started\n");

    initialize_maps();
    load_data();

    int server_sock, client_sock;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_len = sizeof(client_addr);

    // Create socket
    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // Configure server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // Bind socket
    if (bind(server_sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        close(server_sock);
        exit(EXIT_FAILURE);
    }

    // Listen for incoming connections
    if (listen(server_sock, 5) < 0) {
        perror("Listen failed");
        close(server_sock);
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d\n", PORT);

    while (1) {
        // Accept incoming connection
        client_sock = accept(server_sock, (struct sockaddr *)&client_addr, &client_len);
        if (client_sock < 0) {
            perror("Accept failed");
            close(server_sock);
            exit(EXIT_FAILURE);
        }

        // Create thread to handle client
        pthread_t tid;
        int *client_ptr = malloc(sizeof(int));
        *client_ptr = client_sock;
        if (pthread_create(&tid, NULL, clientHandler, (void *)client_ptr) != 0) {
            perror("Failed to create thread");
            close(client_sock);
        }
    }

    close(server_sock);
    return 0;
}