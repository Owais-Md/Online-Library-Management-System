# Library Management System

## Overview
A simple yet effective client-server library management system implemented in C using TCP sockets and a custom hashmap. Designed for straightforward user interaction, this system provides basic functionalities needed for book management and user authentication.
A simple client-server library management system implemented in C using TCP sockets and a custom hashmap implementation.

## Features
- User and admin authentication
- Book borrowing and returning
- Persistent data storage with structured file handling
- User and admin authentication
- Book borrowing and returning
- Persistent data storage

## Installation & Setup

### Cloning the Repository
To download and set up the project, run the following:
```sh
git clone https://github.com/Owais-Md/Online-Library-Management-System.git
cd Online-Library-Management-System
```

### Building & Running
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
- Single-client multiple-server implementation.
- Custom hashmap implementation for efficient key-value storage.
- Data is stored persistently in the `data/` directory.
- Designed for clarity and maintainability, making it easy to extend and modify.
