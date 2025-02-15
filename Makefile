CC = gcc
CFLAGS = -Wall -Wextra -pthread -Wno-unused-parameter -Wno-unused-variable -Wno-return-type
OBJ_DIR = obj
SRC_DIR = src
HEADER_DIR = headers
DATA_DIR = data

SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SRCS))

SERVER_OBJS = $(OBJ_DIR)/server.o $(OBJ_DIR)/implementation.o $(OBJ_DIR)/hashmap.o
CLIENT_OBJS = $(OBJ_DIR)/client.o

BIN_SERVER = server
BIN_CLIENT = client

all: server client

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@ -I$(HEADER_DIR)

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

server: $(SERVER_OBJS)
	$(CC) $(CFLAGS) $^ -o $(BIN_SERVER)

client: $(CLIENT_OBJS)
	$(CC) $(CFLAGS) $^ -o $(BIN_CLIENT)

clean:
	rm -rf $(OBJ_DIR) $(BIN_SERVER) $(BIN_CLIENT)

clear:
	rm -rf $(OBJ_DIR) $(DATA_DIR)/*
