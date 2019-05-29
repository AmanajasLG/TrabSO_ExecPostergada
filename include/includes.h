#ifndef INCLUDES_H_

#define INCLUDES_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <stdbool.h>
#include <libgen.h>
#include <limits.h>

#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/msg.h>
#include <sys/errno.h>
#include <sys/wait.h>
#include <sys/signal.h>
#include <sys/shm.h>

#define KEY_ESCALE 0x03718
#define KEY_NODO_END 0x6659
#define KEY_NODO_FILE 0x8274
#define KEY_ALL_ENDED 0x4543

#define ALL_ENDED_DELTA 20

/* MSG RECEBIDA DO EXEC POST */
struct msg
{
    long sec;
    char arq_executavel[100];
};

/* MSG RECEBIDA DO EXEC POST */
struct msg_all_ended
{
    long id;
    int all_ended;
};

/* MSG ENVIADA PARA OS NÓS */
struct msg_nodo
{
    long pid;
    char arq_executavel[100];
};

/* 
    end info:
        0 -> posição de quem terminou
        1 -> hora de inicio
        2 -> hora de termino 
 */
struct end_msg
{
    long position;
    int end_info[3];
};

struct queue_nodo
{
    time_t init_time;
    time_t end_time;
    int job;
    int sec;
    int origin_sec;
    char arq_executavel[100];
    struct queue_nodo *next;
};

struct queue
{
    struct queue_nodo *init;
};
typedef struct queue Queue;

struct execution_node
{
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
    RUNNING,
};
typedef enum states StateTypes;

enum topology
{
    HYPERCUBE = 0,
    TORUS,
    TREE,
    LIST,
};
typedef enum topology TopologyTypes;

struct nodo
{
    int pid;
    int state;
    int neighbor[4];
};

typedef struct nodo NodoHypercube;
typedef struct nodo NodoTorus;
typedef struct nodo NodoList;

struct tree_nodo
{
    int pid;
    int parent;
    int right;
    int left;
    int state;
};

typedef struct tree_nodo TreeNodo;

int *all_ended, job = 0;
int topology;
NodoHypercube hypercube[16];
NodoTorus torus[16];
TreeNodo tree[15];
NodoList list[3];

#endif