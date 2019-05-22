#ifndef TREE_H_

#define TREE_H_

#include "includes.h"

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

            fattree[left_index].parent = i;
            fattree[i].left = left_index;
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
            fattree[left_index].parent = i;
        }
    }
}

void nodo_loop_tree(int my_position)
{
}

#endif