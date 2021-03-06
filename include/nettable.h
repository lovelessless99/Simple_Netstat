#ifndef _NETTABLE_H
#define _NETTABLE_H

#define BUFFER_SIZE 8192
#define MAX_CONNECTIONS 200

#include <sys/types.h>    
#include <arpa/inet.h>
#include <sys/stat.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <ctype.h>
#include <fcntl.h>
#include <stdio.h>

typedef enum protocal {TCP=2, UDP} Protocal;
typedef enum version {IPv4, IPv6} Version;

typedef struct connection {
        Protocal protocal;
        Version version;
        char local_ip[50];
        char local_port[10];
        char remote_ip[50];
        char remote_port[10];
        char programName[50];
        int  pid;
        int  inode;
} Connection;

void list_connections(Protocal, char*);
void process_traversal();
void hex2ipv6(char *, char *);
void show_infomation(char*);
void ipv4(Protocal);
void ipv6(Protocal);
#endif
