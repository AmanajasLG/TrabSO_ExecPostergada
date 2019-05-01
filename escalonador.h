#ifndef ESCALONADOR_H_
#define ESCALONADOR_H_

#include <stdio.h>
#include <stdlib.h>


 /*Definindo o cabeçalho*/
enum  state{
    BLOCKED = 0,
    READY,
    RUNNING,
};
typedef enum states StateTypes ;


enum topology{
    HYPERCUBE = 0,
    TORUS,
    FATTREE,
};
typedef enum topology TopologyTypes ;


struct nodo{
    int pid;
    int state;
};

typedef struct nodo Nodo;

struct tree_nodo{
    int pid;
    int parent;
    int right;
    int right_extra;
    int left;
    int left_extra;
};

typedef struct tree_nodo TreeNodo;


void print_topology(int type, TreeNodo* fattree){
    if(type == FATTREE){
        for(int i = 0; i < 15; i++){
            printf("nodo [%d] pai: %d dir %d|%d esq %d|%d\n", i, fattree[i].parent, fattree[i].right, fattree[i].right_extra, fattree[i].left, fattree[i].left_extra );
        }
        
    }
}

#endif 