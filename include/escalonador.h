#ifndef ESCALONADOR_H_
#define ESCALONADOR_H_

#include <stdio.h>
#include <stdlib.h>

#define KEY_EXEC_POST 0x03718
#define KEY_ESCALE 0x6659

struct msg{
    long sec;
    char arq_executavel[100];
};

struct end_msg{
    long pid;
    time_t exec_time[2];
};

struct queue_nodo{
    int sec;
    char arq_executavel[100];
    struct queue_nodo* next;
};

struct queue {
    struct queue_nodo* init;
};
typedef struct queue Queue;


struct execution_node{
    long sec;
    char arq_executavel[100];
    time_t exec_init;
    time_t exec_end;
};

/*Definindo o cabeçalho*/
enum state
{
    BLOCKED = 0,
    READY,
    RUNNING
};
typedef enum states StateTypes;

enum topology
{
    HYPERCUBE = 0,
    TORUS,
    FATTREE,
};
typedef enum topology TopologyTypes;

struct nodo
{
    int pid;
    int state;
};

typedef struct nodo Nodo;

struct tree_nodo
{
    int pid;
    int parent;
    int right;
    int right_extra;
    int left;
    int left_extra;
    int state;
};

typedef struct tree_nodo TreeNodo;


void print_topology(int type, TreeNodo *fattree)
{

    switch (type)
    {
    case HYPERCUBE:
        break;
    case TORUS:
        break;
    case FATTREE:
        for (int i = 0; i < 15; i++)
            printf("nodo [%d] pid %d pai: %d dir %d|%d esq %d|%d\n", i, fattree[i].pid, fattree[i].parent, fattree[i].right, fattree[i].right_extra, fattree[i].left, fattree[i].left_extra);
        break;
    }
}

void create_hypercube(Nodo hypercube[16]) {}
void create_torus(Nodo torus[4][4]) {}

void create_tree(TreeNodo fattree[15])
{
    for (int i = 0; i < 15; i++)
    {
        int right_index = 2 * i + 2;
        int left_index = 2 * i + 1;

        if (i <= 2)
        {
            if (i == 0)
            {
                fattree[i].parent = -1;
            }

            fattree[right_index].parent = i;
            fattree[i].right = right_index;
            fattree[i].right_extra = right_index;

            fattree[left_index].parent = i;
            fattree[i].left = left_index;
            fattree[i].left_extra = left_index;
        }
        else
        {
            if (i < 7)
            {
                fattree[i].right = right_index;
                fattree[i].left = left_index;
            }
            else
            {
                fattree[i].right = -1;
                fattree[i].left = -1;
            }

            fattree[right_index].parent = i;
            fattree[i].right_extra = -1;
            fattree[left_index].parent = i;
            fattree[i].left_extra = -1;
        }
    }
}

Queue* start_queue(){
    Queue* ready_queue = (Queue*) malloc(sizeof(Queue));
    if(ready_queue != NULL){
        ready_queue->init = NULL;
    }
    return ready_queue;
}

int is_empty(Queue* ready_queue){
    if(ready_queue->init == NULL)
        return 1;
    else
        return 0;
}

int free_queue(Queue* ready_queue){
    struct queue_nodo* aux, *tmp= ready_queue->init;
    if(!is_empty(ready_queue)){
        do{
            aux = tmp;
            tmp = tmp->next;
            free(aux);
            
        }while(tmp != NULL);
    }

}
int remove_queue(Queue* ready_queue){
    if(ready_queue == NULL)
        return 0;
    if(ready_queue->init == NULL){
        return 0;
    }
    struct queue_nodo* nodo = ready_queue->init;

    ready_queue->init = ready_queue->init->next;

    free(nodo);
    return 1;

}

int insert_queue(Queue* ready_queue, struct msg insert_msg){
    if (ready_queue == NULL)
        return 0;
    struct queue_nodo* new_nodo = (struct queue_nodo *) malloc(sizeof(struct queue_nodo));
    
    if(new_nodo == NULL)
        return 0;
    new_nodo->sec = insert_msg.sec; 
    strcpy(new_nodo->arq_executavel, insert_msg.arq_executavel);
    new_nodo->next = NULL;
    printf("New nodo %d %s\n",new_nodo->sec, new_nodo->arq_executavel);
    if(ready_queue->init == NULL){//empty queue
        printf("queue init null :(\n");
        ready_queue->init = new_nodo;
    }else{
        struct queue_nodo* tmp = ready_queue->init;
        struct queue_nodo* previous = NULL;
    
        do{
            if(tmp->sec <= new_nodo->sec){
                previous = tmp;
                tmp = tmp->next;   
            }
            else{
                struct queue_nodo* aux = NULL;
                if(previous != NULL){
                    aux = previous->next;
                    previous->next = new_nodo;
                    new_nodo->next = aux;
                }else{
                    new_nodo->next = ready_queue->init;
                    ready_queue->init = new_nodo;
                }
                break;
            }
        }while(tmp != NULL);
        if(tmp == NULL){
            previous->next = new_nodo;
            new_nodo->next = NULL;
        }
    }
    
    return 1;

}
void print_queue(Queue* ready_queue){
    struct queue_nodo* tmp = ready_queue->init;
    if(!is_empty(ready_queue)){
        do{
            printf("Nodo %d -> ", tmp->sec); 
            tmp = tmp->next;
        }while(tmp != NULL);

    }

    printf("\n");

}
#endif