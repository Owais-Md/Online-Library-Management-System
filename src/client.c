#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define PORT 8080
#define BUFFER_SIZE 1024

// Function to send a message to the server
void send_message(int sock, const char *msg) {
    send(sock, msg, strlen(msg), 0);
}

// Function to receive a message from the server
void receive_message(int sock, char *buffer) {
    recv(sock, buffer, BUFFER_SIZE, 0);
}

void take_line_input(char string[BUFFER_SIZE]){
    memset(string, 0, BUFFER_SIZE);
    fgets(string, BUFFER_SIZE, stdin);
    string[strcspn(string, "\n")] = '\0';
}

int main() {
    int sock = 0;
    struct sockaddr_in serv_addr;
    char buffer[BUFFER_SIZE];

    // Create socket
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation error");
        return -1;
    }

    // Configure server address
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Convert IPv4 and IPv6 addresses from text to binary form
    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        perror("Invalid address / Address not supported");
        return -1;
    }

    // Connect to server
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Connection Failed");
        return -1;
    }

    char prompt[BUFFER_SIZE];
    char response[BUFFER_SIZE];
    
    // receive prompts till server doesn't kill thread, respond to prompts
    while(1){
        receive_message(sock, prompt);
        send_message(sock, "receiving message");
        if(strncmp(prompt, "prompt", 6) == 0){
            receive_message(sock, prompt);
            printf("%s", prompt);
            take_line_input(response);
            send_message(sock, response);
        }
        else if(strncmp(prompt, "output", 6) == 0){
            receive_message(sock, response);
            printf("%s", response);
            if(strcmp(response, "Exiting program\n") == 0) exit(0);
        }
    }

    close(sock);
    return 0;
}
