#ifndef _NETTABLE_H
#define _NETTABLE_H

typedef enum protocal {TCP, UDP} Protocal;

typedef struct netTable {
        Protocal protocal;
}netTable;

void init_netTable();


#endif
