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

#define KEY_ESCALE 0x03718
#define KEY_NODO_END 0x6659
#define KEY_NODO_FILE 0x8274

int pid_son_process = 0;

void end_node()
{
    int status;

    if (pid_son_process != 0)
    {
        kill(SIGKILL, pid_son_process);
        wait(&status);
    }
    exit(0);
}

/* MSG RECEBIDA DO EXEC POST */
struct msg
{
    long sec;
    char arq_executavel[100];
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
    int msg_rcv_number;
    int neighbor[2];
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
    int msg_rcv_number;
};

typedef struct tree_nodo TreeNodo;

int job = 0;
int topology;
NodoHypercube hypercube[16];
NodoTorus torus[16];
TreeNodo tree[15];
NodoList list[3];

#endif