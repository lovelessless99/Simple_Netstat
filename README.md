# Simple_Netstat

### Abstraction
This is a simple netstat(netstat-like). The program lists all the existing TCP and UDP connections. For each identified connection (socket descriptor), find the corresponding process name and its command lines that creates the connection (socket descriptor).

### Usage
        .simple_ns [-t|--tcp] [-u|--udp] [filter-string]

* -t or --tcp: list only TCP connections.
* -u or --udp: list only UDP connections.
* An optional string to list only command lines that containing the string.

* When no argument is passed, program will output all identified connections. You may test program with a root account so that your program would be able to access /proc files owned by other users

## Code Comment

#### 1. Set the options 

```c 
        const char* short_options = "tuhf:";
        const struct option long_options[] = {
                {"tcp" , no_argument, NULL, 't'},
                {"udp" , no_argument, NULL, 'u'},
                {"help", no_argument, NULL, 'h'},
                {0, 0, 0, 0}
        };
```
and use `getopt_long` function call

#### 2.Traverse all process and find the socket file

(1) Traverse `/proc/[pid]/stat`, and will see all process `filename` and `process id`

(2) Traverse `/proc/[pid]/fd`, and you can `read all symbolic link to the file`, and you can get the inode of each socket.

(3) When you want to get all connection, you need to open the following file:
        

        /proc/net/tcp
        /proc/net/tcp6
        /proc/net/udp
        /proc/net/udp6

and you can get the local_IP, local_Port, Remote_IP, Remote_Port with `HEX_STRING`, **When translating ip_address, you must translate big-endian to little endian !**

(4) Function You Should Work With
``` c
        1. getopt_long()
        2. opendir(), readdir(), readlink(), sscanf(), stat()
        3. inet_ntop()
```


## Future Work
1. Filter String for `Regular Expression (regex.h)`
2. use `fork()` to make the program as a shell.
