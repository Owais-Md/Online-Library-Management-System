# Library Management System

## Overview
A simple yet effective client-server library management system implemented in C using TCP sockets and a custom hashmap. Designed for straightforward user interaction, this system provides basic functionalities needed for book management and user authentication.

## Features
- User and admin authentication
- Book borrowing and returning
- Persistent data storage with structured file handling
- Command-line based interaction for ease of use

## Installation & Usage
```sh
make all    # Build server and client
./server    # Run server
./client    # Run client
```

## Project Structure
```
.
├── src        # Source files
├── headers    # Header files
├── obj        # Compiled object files
├── data       # Persistent storage
├── Makefile   # Build system
└── README.md  # Documentation
```

## Build System
```sh
make all        # Builds server and client
make server     # Builds only the server
make client     # Builds only the client
make clean      # Removes all compiled binaries and object files
make clean_temp # Removes object files and temporary data, keeps binaries
```

## Notes
- Single-client server implementation.
- Custom hashmap implementation for efficient key-value storage.
- Data is stored persistently in the `data/` directory.
- Designed for clarity and maintainability, making it easy to extend and modify.
