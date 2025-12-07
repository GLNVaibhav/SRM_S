# Student Record Management System (SRMS)

## Overview
SRMS is a comprehensive C-based student database management system with role-based access control, CRUD operations, ticket management, and advanced analytics features. The system provides separate interfaces for administrators and students with secure authentication and persistent data storage.

## Features

### Core Features
- **Role-Based Access Control**: Separate login interfaces for administrators and students
- **Student Management**: Complete CRUD (Create, Read, Update, Delete) operations
- **Authentication**: Secure login with username/password for admins and ID/password for students
- **Data Persistence**: Binary file storage with automatic CSV/TXT exports
- **Ticket System**: Students can raise tickets; admins can view and close them
- **Analytics**: Comprehensive statistics including average CGPA, highest/lowest performers
- **Sorting**: Multiple sorting options (by CGPA ascending/descending, by name A-Z)
- **Data Validation**: Safe input handling with no buffer overflow vulnerabilities

### File Management
- **Binary Storage**: `students.dat` and `tickets.dat` for efficient data persistence
- **CSV Export**: Automatic generation of `students.csv` for spreadsheet compatibility
- **Text Export**: Human-readable `students.txt` from the "View All Students" feature

## System Requirements
- **Compiler**: GCC with C99 support or higher
- **OS**: Windows/Linux/macOS
- **Libraries**: Standard C library, Math library (for potential extensions)

## Compilation

Navigate to the project directory and compile using:

```bash
gcc -std=c99 -O2 -Wall -Wextra -lm SRMS.c -o SRMS
