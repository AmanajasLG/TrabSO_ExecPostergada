#ifndef ESCALONADOR_H_
#define ESCALONADOR_H_

#include <stdio.h>
#include <stdlib.h>

#define KEY 0x3718


struct msg{
    long mtype;
    struct postergado* post;
};

struct postergado {
    char arq_executavel[100];
    int sec;
};
 /*Definindo o cabeçalho*/
enum state{
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
    int state;
};

typedef struct tree_nodo TreeNodo;

void print_topology(int type, TreeNodo* fattree){

     switch (type){ 
        case HYPERCUBE:
            break;
        case TORUS:
            break;
        case FATTREE:
            for(int i = 0; i < 15; i++)
                printf("nodo [%d] pid %d pai: %d dir %d|%d esq %d|%d\n", i,fattree[i].pid, fattree[i].parent, fattree[i].right, fattree[i].right_extra, fattree[i].left, fattree[i].left_extra );            
            break;
    }

}

#endif 