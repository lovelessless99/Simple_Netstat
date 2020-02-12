#include "nettable.h"
#include "main.h"


void help(){
    printf("Usage: ./hw1 [ OPTIONS ]\n");
    printf("       ./hw1 [ OPTIONS ] [ FILTER ]\n");
    printf("  -t, --tcp              List TCP connection\n");
    printf("  -u, --udp              List UDP connection\n");
    printf("  -h, --help             this essage\n\n");
}

int main(int argc, char **argv)
{
        const char* short_options = "tuhf:";
        const struct option long_options[] = {
                {"tcp" , no_argument, NULL, 't'},
                {"udp" , no_argument, NULL, 'u'},
                {"help", no_argument, NULL, 'h'},
                {"filter", required_argument, NULL, 'f'},
                {0, 0, 0, 0}
        };

        int option;
        if(argc > 2) {
                while( ( option = getopt_long(argc, argv, short_options, long_options, NULL) ) != -1 )
                {
                        switch(option){
                                case 'h': help(); break;
                                case 't': list_connections(TCP); break;
                                case 'u': list_connections(UDP); break;
                                case 'f': printf("Filter!\n")  ; break;
                                default : help();
                        }
                }
        }
        else { list_connections(TCP); list_connections(UDP);}
        
        char *filterStr = strdup(argv[argc-1]);
        

        exit(EXIT_SUCCESS);
}
