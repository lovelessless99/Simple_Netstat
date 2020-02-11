#include "nettable.h"
#include "main.h"

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
        while( ( option = getopt_long(argc, argv, short_options, long_options, NULL) ) != -1 )
        {
                switch(option){
                        case 't': printf("TCP! enumTCP = %d\n", TCP); break;
                        case 'u': printf("UDP! enumUDP = %d\n", UDP); break;
                        case 'f': printf("Filter!\n"); break;
                        default: printf("help!\n");
                }
        }

        exit(EXIT_SUCCESS);
}
