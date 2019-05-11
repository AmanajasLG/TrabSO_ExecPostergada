#ifndef ESCALONADOR_H_
#define ESCALONADOR_H_

#include <stdio.h>
#include <stdlib.h>

#define KEY_EXEC_POST 0x03718
#define KEY_ESCALE 0x6659

struct msg
{
    long sec;
    char arq_executavel[100];
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

#endif