#include "nettable.h"
#include "main.h"


void help(){
    printf("Usage: ./simple_ns [ OPTIONS ]\n");
    printf("       ./simple_ns [ OPTIONS ] [ FILTER ]\n");
    printf("  -t, --tcp              List TCP connection\n");
    printf("  -u, --udp              List UDP connection\n");
    printf("  -h, --help             help tip\n\n");
}

int main(int argc, char **argv)
{
        const char* short_options = "tuhf:";
        const struct option long_options[] = {
                {"tcp" , no_argument, NULL, 't'},
                {"udp" , no_argument, NULL, 'u'},
                {"help", no_argument, NULL, 'h'},
                {0, 0, 0, 0}
        };
        char *filterStr = (argc > 1)? (!strstr(argv[argc-1], "-"))? strdup(argv[argc-1]) : NULL : NULL;

        int option;
        if(argc >= 2) {
                while( ( option = getopt_long(argc, argv, short_options, long_options, NULL) ) != -1 )
                {
                        switch(option){
                                case 't': list_connections(TCP, filterStr); break;
                                case 'u': list_connections(UDP, filterStr); break;
                                default : help();
                        }
                }
        }
        else { list_connections(TCP, filterStr); list_connections(UDP, filterStr);}
        
        exit(EXIT_SUCCESS);
}
