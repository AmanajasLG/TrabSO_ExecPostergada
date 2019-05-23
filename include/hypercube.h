#ifndef HYPERCUBE_H_

#define HYPERCUBE_H_

#include "includes.h"

void create_hypercube(NodoHypercube hypercube[16])
{
    for (int i = 0; i < 16; i++)
    {

        hypercube[i].neighbor[0] = i ^ 0b0001;
        hypercube[i].neighbor[1] = i ^ 0b0010;
        hypercube[i].neighbor[2] = i ^ 0b0100;
        hypercube[i].neighbor[3] = i ^ 0b1000;
    }
}

void print_hypercube(NodoHypercube hypercube[16])
{

    for (int i = 0; i < 16; i++)
    {

        for (int j = 0; j < 4; j++)
        {
            printf("no %d - vizinho %d: %d\n ", i, j, hypercube[i].neighbor[j]);
        }
        printf("\n\n");
    }
}

void nodo_loop_hypercube()
{
}

void nodo_0_loop_hypercube()
{
}

#endif