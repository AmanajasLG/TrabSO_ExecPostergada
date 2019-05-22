#ifndef TORUS_H_

#define TORUS_H_

#include "includes.h"

void create_torus(NodoTorus torus[16])
{
    int x, y;
    int index = 0;
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {

            x = i;
            y = abs((j - 3) % 4);
            torus[index].neighbor[0] = x * 4 + y;

            x = i;
            y = j % 2 == 0 ? (j + 1) % 4 : abs((j - 1) % 4);
            torus[index].neighbor[1] = x * 4 + y;

            x = abs((i - 3) % 4);
            y = j;
            torus[index].neighbor[2] = x * 4 + y;

            x = i % 2 == 0 ? (i + 1) % 4 : abs((i - 1) % 4);
            y = j;
            torus[index].neighbor[3] = x * 4 + y;

            index++;
        }
    }
}

void print_torus(NodoTorus torus[16])
{
    for (int i = 0; i < 16; i++)
    {
        for (int k = 0; k < 4; k++)
        {
            printf("no %d- vizinho %d: [%d]\n", i, k, torus[i].neighbor[k]);
        }

        printf("\n\n");
    }
}

void nodo_loop_torus()
{
}

#endif