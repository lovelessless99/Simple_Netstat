#include "nettable.h"

Connection connection[MAX_CONNECTIONS];
int count = 0;

void list_connections(Protocal protocal)
{
        
        if(protocal == TCP){
                ipv6(TCP);
        
        }
        
        if(protocal == UDP){


        }

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
                sscanf(line,"%*s %x:%s %x:%s %*s %*s %*s %*s %*s %*s %d %*s ", &hex_localAddr, &hex_localPort, &hex_remoteAddr, &hex_remotePort, &inode);
                
                sprintf(localPort, "%ld", strtol(hex_localPort, NULL, 16));
                sprintf(remotePort, "%ld", strtol(hex_remotePort, NULL, 16));

                // Method 1 : order is inverse !
                sprintf(localAddr  , "%d.%d.%d.%d" , hex_localAddr & 0x000000FF , (hex_localAddr  & 0x0000FF00) >> 8, (hex_localAddr  & 0x00FF0000) >> 16, (hex_localAddr  & 0xFF000000) >> 24 );
                sprintf(remoteAddr  , "%d.%d.%d.%d", hex_remoteAddr & 0x000000FF, (hex_remoteAddr & 0x0000FF00) >> 8, (hex_remoteAddr & 0x00FF0000) >> 16, (hex_remoteAddr & 0xFF000000) >> 24 );
                
                // Method 2 :
                // struct in_addr ipPack;

                // ipPack.s_addr = hex_localAddr;
                // inet_ntop(AF_INET, &ipPack, localAddr, INET_ADDRSTRLEN);

                // ipPack.s_addr = hex_remoteAddr;
                // inet_ntop(AF_INET, &ipPack, remoteAddr, INET_ADDRSTRLEN);

                connection[count].protocal = protocal;
                connection[count].version = IPv4;
                connection[count].inode = inode;

                strcpy(connection[count].local_ip, localAddr);
                strcpy(connection[count].local_port, localPort);
                strcpy(connection[count].remote_ip, remoteAddr);
                strcpy(connection[count].remote_port, remotePort);
                count++;

                line = strtok(NULL, "\n");
        }
        // process_traversal(connection, count);
}


void ipv6(Protocal protocal)
{
        char buffer[BUFFER_SIZE] = {0};
        char file[20] = {0};
        (protocal == TCP)? strcpy(file, "/proc/net/tcp6") : strcpy(file, "/proc/net/udp6");

        int fd = open(file, O_RDONLY);
        int size = read(fd, buffer, BUFFER_SIZE);
        
        char hex_localAddr[50];
        char hex_localPort[10], localPort[10];

        char hex_remoteAddr[50];
        char hex_remotePort[10], remotePort[10];

        int inode;
        
        char *line = strtok(buffer, "\n");
        line = strtok(NULL, "\n"); // remove header 
        
        // 8802:0120:50D7:0140:36A3:B17C:A9BA:35F1

        while(line != NULL)
        {       
                
                sscanf(line,"%*s %[^:]%s %[^:]%s %*s %*s %*s %*s %*s %*s %d %*s ", &hex_localAddr, &hex_localPort, &hex_remoteAddr, &hex_remotePort, &inode);
                
                sprintf(localPort, "%ld", strtol(hex_localPort, NULL, 16));
                sprintf(remotePort, "%ld", strtol(hex_remotePort, NULL, 16));
                

                char local_ip[100] = {0};
                char remote_ip[100] = {0};
                struct in6_addr ip_localPack, ip_remotePack;

                for(int i = 0 ; i < 16; i++ )
                {
                        sscanf(hex_localAddr  + 2*i, "%2hhx", &ip_localPack.s6_addr[i]);
                        sscanf(hex_remoteAddr + 2*i, "%2hhx", &ip_remotePack.s6_addr[i]);
                }

                inet_ntop(AF_INET6, &ip_localPack ,local_ip, INET6_ADDRSTRLEN);
                inet_ntop(AF_INET6, &ip_remotePack,remote_ip, INET6_ADDRSTRLEN);
                

                // printf("ipv6:\tremote address = %s:%s\tlocal address = %s:%s \n", hex_remoteAddr, hex_remotePort, hex_localAddr, hex_localPort);
                printf("ipv6:\tlocal address = %s:%s\tremote address = %s:%s \n", local_ip, localPort, remote_ip, remotePort);

                // connection[count].protocal = protocal;
                // connection[count].version = IPv6;
                // connection[count].inode = inode;

                // strcpy(connection[count].local_ip, localAddr);
                // strcpy(connection[count].local_port, localPort);
                // strcpy(connection[count].remote_ip, remoteAddr);
                // strcpy(connection[count].remote_port, remotePort);
                // count++;

                line = strtok(NULL, "\n");
        }
        // process_traversal(connection, count);


}


void process_traversal(Connection* connection, int count)
{
        DIR *rootdir = opendir("/proc");
        struct dirent* process_dir;
        
        while( (process_dir = readdir(rootdir)) )
        {
                if(!isdigit( process_dir->d_name[0] )) continue; // check if not process directory by first element
                
                // get filename and process id
                char process_stat_path[50] = {0};
                char buffer[50] = {0};
                char filename[20] = {0};
                pid_t pid;
                sprintf(process_stat_path, "/proc/%s/stat", process_dir->d_name);
                int statfd = open(process_stat_path, O_RDONLY);
                read(statfd, buffer, 50);
                sscanf(buffer, "%d (%[^)]", &pid, &filename);
                // printf("pid = %d\t, filename = %s\n", pid, filename);


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
                                                                printf("%s:%s\t%s:%s\t\t%d/%s\n", connection[i].local_ip, connection[i].local_port, 
                                                                                                           connection[i].remote_ip, connection[i].remote_port, 
                                                                                                           connection[i].pid, connection[i].programName);
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
