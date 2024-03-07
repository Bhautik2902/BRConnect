# Project: BRConnect

## Description
This project is a client-server application where clients can request files or a set of files from a server. The server, running on a separate machine, handles multiple client requests by searching its file system, rooted at its home directory, and returns the requested files or directories. The project includes implementing custom commands for file and directory management between the client and server using socket programming in C.

### Features
- **Client-Server Communication**: Utilizes socket programming to enable communication between multiple clients and a server.
- **File Management**: Allows clients to request specific files, lists of files, or files within certain parameters (size, date, type) from the server.
- **Dynamic Request Handling**: The server can handle multiple requests simultaneously by forking a new process for each client connection.
- **Directory Browsing**: Clients can request lists of directories based on different sorting criteria such as alphabetical order or creation time.
- **Data Compression**: Files sent from the server to the client are compressed into a `.tar.gz` format to optimize transfer speed and bandwidth usage.
- **Robust Error Handling**: Both the server and client have mechanisms to handle errors and provide meaningful feedback to the user.

### Technical Specifications
- **Languages**: C
- **Protocols**: TCP/IP for socket communication
- **Tools and Libraries**: Standard C libraries, POSIX API for process and thread management

### Project Structure
The project is divided into several components:
1. `serverw24.c`: The main server program handling incoming connections and requests.
2. `clientw24.c`: The client program allowing users to send commands to the server.
3. `mirror1.c` and `mirror2.c`: Additional server instances for load distribution and redundancy.

### How to Run
Detailed instructions on how to compile and run the server and client programs will be provided in the README files within each component's directory.

### Contributing
This project is developed as part of the COMP-8567 course requirements. Contributions are welcome but must adhere to the project guidelines and coding standards.

### License
This project is licensed under the MIT License - see the LICENSE file for details.

---

### Installation and Usage
Before running the application, ensure you have a C compiler installed on your system. The project has been tested with GCC.

1. Clone the repository to your local machine.
2. Navigate to each component's directory and compile the source code using GCC. For example:
   ```bash
   gcc -o serverw24 serverw24.c
   gcc -o clientw24 clientw24.c
   gcc -o mirror1 mirror1.c
   gcc -o mirror2 mirror2.c
   ```
3. Start the server (and mirrors) on separate machines or terminals:
   ```bash
   ./serverw24
   ./mirror1
   ./mirror2
   ```
4. Run the client application from a different terminal or machine:
   ```bash
   ./clientw24
   ```
5. Follow the on-screen prompts to send commands to the server.

### Commands
Clients can use the following commands to interact with the server:
- `dirlist -a`: Retrieve a list of all subdirectories in alphabetical order.
- `dirlist -t`: Retrieve a list of all subdirectories sorted by creation time.
- `w24fn [filename]`: Retrieve information about a specific file.
- `w24fz [size1] [size2]`: Retrieve files within a specific size range.
- `w24ft [extension list]`: Retrieve files matching a set of extensions.
- `w24fdb [date]`: Retrieve files created before a specific date.
- `w24fda [date]`: Retrieve files created after a specific date.
- `quitc`: Disconnect from the server.

### Future Work
- Implement SSL/TLS for secure communication.
- Add support for parallel file transfers.
- Extend the file filtering capabilities.

### Support
For any technical issues or contributions, please open an issue or pull request on GitHub. For project-related queries, contact the course instructor or project contributors.

Remember, this project is subject to academic integrity rules. Ensure your contributions are original and cite any external sources used.
