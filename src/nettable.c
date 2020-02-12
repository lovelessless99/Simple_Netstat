#include "nettable.h"

Connection connection[MAX_CONNECTIONS];
int count = 0;

void list_connections(Protocal protocal)
{
        if(protocal == TCP){ ipv4(TCP); ipv6(TCP); }
        if(protocal == UDP){ ipv4(UDP); ipv6(UDP); }

        (protocal == TCP)? printf("List of TCP Connections:\n") : printf("List of UDP Connections:\n");
        printf("%-5s   %-28s %-30s %s\n","Proto","Local Address","Foreign Address","PID/Program name and arguments");
        // printf("Proto\tLocal Address\tForeign Address\tPID/Program name and arguments\n");
        process_traversal();
        show_infomation();
        
        // reset
        memset(connection, 0, sizeof(Connection) * MAX_CONNECTIONS);
        count = 0;
}

void ipv4(Protocal protocal)
{
        char buffer[BUFFER_SIZE] = {0};

        // const char file [] = (protocal == TCP)? "/proc/net/tcp" : "/proc/net/udp" ;
        
        char file[20] = {0};
        (protocal == TCP)? strcpy(file, "/proc/net/tcp") : strcpy(file, "/proc/net/udp");


        int fd = open(file, O_RDONLY);
        int size = read(fd, buffer, BUFFER_SIZE);
        
        unsigned int hex_localAddr, hex_remoteAddr;
        char localAddr[50], remoteAddr[50];

        char hex_localPort[10], localPort[10];
        char hex_remotePort[10], remotePort[10];
        int inode;
        
        char *line = strtok(buffer, "\n");
        line = strtok(NULL, "\n"); // remove header 
        
        

        while(line != NULL)
        {       
                sscanf(line,"%*s %x:%s %x:%s %*s %*s %*s %*s %*s %*s %d %*s ", &hex_localAddr, &hex_localPort, &hex_remoteAddr, &hex_remotePort, &connection[count].inode);

                connection[count].protocal = protocal;
                connection[count].version = IPv4;

                sprintf(connection[count].local_port, "%ld", strtol(hex_localPort, NULL, 16));
                sprintf(connection[count].remote_port, "%ld", strtol(hex_remotePort, NULL, 16));

                // Method 1 : order is Bigendian, so we need to translate to little endian !
                // sprintf(localAddr  , "%d.%d.%d.%d" , hex_localAddr & 0x000000FF , (hex_localAddr  & 0x0000FF00) >> 8, (hex_localAddr  & 0x00FF0000) >> 16, (hex_localAddr  & 0xFF000000) >> 24 );
                // sprintf(remoteAddr  , "%d.%d.%d.%d", hex_remoteAddr & 0x000000FF, (hex_remoteAddr & 0x0000FF00) >> 8, (hex_remoteAddr & 0x00FF0000) >> 16, (hex_remoteAddr & 0xFF000000) >> 24 );
                
                // Method 2 :
                struct in_addr ipPack;

                ipPack.s_addr = hex_localAddr;
                inet_ntop(AF_INET, &ipPack, connection[count].local_ip, INET_ADDRSTRLEN);

                ipPack.s_addr = hex_remoteAddr;
                inet_ntop(AF_INET, &ipPack, connection[count].remote_ip, INET_ADDRSTRLEN);
                count++;

                line = strtok(NULL, "\n");
        }
}


void ipv6(Protocal protocal)
{
        char buffer[BUFFER_SIZE] = {0};
        char file[20] = {0};
        (protocal == TCP)? strcpy(file, "/proc/net/tcp6") : strcpy(file, "/proc/net/udp6");

        int fd = open(file, O_RDONLY);
        int size = read(fd, buffer, BUFFER_SIZE);
           
        char *line = strtok(buffer, "\n");
        line = strtok(NULL, "\n"); // remove header 
        
     
        while(line != NULL)
        {       
                char hex_localAddr[50], hex_localPort[10], hex_remoteAddr[50], hex_remotePort[10];
                sscanf(line,"%*s %[^:]%s %[^:]%s %*s %*s %*s %*s %*s %*s %d %*s ", &hex_localAddr, &hex_localPort, &hex_remoteAddr, &hex_remotePort, &connection[count].inode);
                
                connection[count].protocal = protocal;
                connection[count].version = IPv6;
                
                sprintf(connection[count].local_port,  "%ld", strtol(hex_localPort,  NULL, 16));
                sprintf(connection[count].remote_port, "%ld", strtol(hex_remotePort, NULL, 16));
                
                hex2ipv6(hex_localAddr, connection[count].local_ip);
                hex2ipv6(hex_remoteAddr, connection[count].remote_ip);
                
                line = strtok(NULL, "\n");
                count++;
        }
}

void hex2ipv6(char *hex_address, char *ip)
{
        struct in6_addr ip_Pack;

        /* Big Endian to Little Endian */

        for(int i = 0 ; i < 4; i++ )
        {
                char tmp[8] = {0};
                sscanf(hex_address + 8*i, "%8s", &tmp);
                long address = strtol(tmp, NULL, 16);
                ip_Pack.s6_addr[4*i+3] = (address & 0xFF000000) >> 24;
                ip_Pack.s6_addr[4*i+2] = (address & 0x00FF0000) >> 16;
                ip_Pack.s6_addr[4*i+1] = (address & 0x0000FF00) >> 8 ;
                ip_Pack.s6_addr[4*i  ] = (address & 0x000000FF); 
        
        }
        
        inet_ntop(AF_INET6, &ip_Pack ,ip, INET6_ADDRSTRLEN);
}

void process_traversal()
{
        DIR *rootdir = opendir("/proc");
        struct dirent* process_dir;
        
        while( (process_dir = readdir(rootdir)) )
        {
                if(!isdigit( process_dir->d_name[0] )) continue; // check if not process directory by first element
                
                // get filename and process id
                char process_stat_path[50] = {0};
                char buffer[1000] = {0};
                char filename[1000] = {0};
                pid_t pid;

                sprintf(process_stat_path, "/proc/%s/stat", process_dir->d_name);
                int statfd = open(process_stat_path, O_RDONLY);
                read(statfd, buffer, 50);
                sscanf(buffer, "%d (%[^)]", &pid, &filename);

                // ** if you want to get parameter, go to parse /proc/pid/cmdline files **
                // sprintf(process_stat_path, "/proc/%s/cmdline", process_dir->d_name);
                // int cmdfd = open(process_stat_path, O_RDONLY);
                // sscanf(process_dir->d_name, "%d", &pid);
                // read(cmdfd, filename, 1000);
                // sscanf(buffer, "%s",&filename);

                // get all reference inode
                char fd_dirpath[50] = {0};
                sprintf(fd_dirpath, "/proc/%s/fd", process_dir->d_name);
                
                DIR* fd_dir = opendir(fd_dirpath);
                if(fd_dir) {
                        struct dirent* fd;
                        while( fd = readdir(fd_dir))
                        {
                                if(!isdigit(fd->d_name[0])) continue;
                                char fd_path[50] = {0};
                                char buf[50] = {0};
                                sprintf(fd_path, "%s/%s", fd_dirpath, fd->d_name);
                                
                                if(readlink(fd_path, buf, 50))
                                {
                                        if(strstr(buf, "socket") != NULL){
                                                int inode_num;
                                                sscanf(buf," socket:[%d]", &inode_num);
                                                
                                                for(int i = 0 ; i < count; i++)
                                                {
                                                        if(inode_num == connection[i].inode)
                                                        {
                                                                connection[i].pid = pid;
                                                                strcpy(connection[i].programName, filename); 
                                                        }
                                                }
                                        }
                                }
                        }
                }
                closedir(fd_dir);
        }
        closedir(rootdir);
}

void show_infomation()
{
        for(int i = 0 ; i < count; i++)
        {
                if ( connection[i].protocal == TCP ){
                        if (connection[i].version == IPv4) printf("tcp\t");
                        else printf("tcp6\t");
                }
                else if ( connection[i].protocal == UDP){
                        if (connection[i].version == IPv4) printf("udp\t");
                        else printf("udp6\t");
                }

                if(atoi(connection[i].local_port) == 0){ 
                        memset(connection[i].local_port, 0, 10);
                        strcpy(connection[i].local_port, "*");
                }
                if(atoi(connection[i].remote_port) == 0){
                        memset(connection[i].remote_port, 0, 10);
                        strcpy(connection[i].remote_port, "*");
                }

                printf("%s:%-25s%s:%-15s\t%d/%-20s\n", 
                        connection[i].local_ip, connection[i].local_port, 
                        connection[i].remote_ip, connection[i].remote_port, 
                        connection[i].pid, connection[i].programName
                );
        
        }
        printf("\n");
}