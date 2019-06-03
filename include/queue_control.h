/**
 * @authors: 
 * @name Luíza Amanajás
 * @matricula 160056659
 * @name Yuri Castro do Amaral
 * @matricula 140033718
 */

#ifndef QUEUE_CONTROL_H_

#define QUEUE_CONTROL_H_

#include "includes.h"

#define QUEUE_SIZE 20

int ready_queue_size = 0;
int run_queue_size = 0;
int ended_queue_size = 0;

struct queue_nodo queue_matrix[3][QUEUE_SIZE];

void print_matrix()
{

    for (int i = 0; i < 3; i++)
    {
        if (i == READY)
        {
            printf("READY QUEUE [%d] -> ", ready_queue_size);
        }
        else if (i == RUN)
        {
            printf("RUN QUEUE [%d] -> ", run_queue_size);
        }
        else
        {
            printf("ENDED QUEUE [%d] -> ", ended_queue_size);
        }

        for (int j = 0; j < QUEUE_SIZE; j++)
        {
            if ((i == READY && j < ready_queue_size) || (i == RUN && j < run_queue_size) || (i == ENDED && j < ended_queue_size))
            {
                printf("Nodo %d(%d | %d) -> ", queue_matrix[i][j].sec, queue_matrix[i][j].origin_sec, queue_matrix[i][j].job);
            }
            else
            {
                if (j == 0)
                {
                    printf("EMPTY");
                }
                printf("\n");
                break;
            }
        }
    }
}

void print_nodo(struct queue_nodo *nodo)
{
    if (nodo != NULL)
    {
        printf("PRINT NODO %d(%d | %p)\n", nodo->sec, nodo->origin_sec, nodo);
    }
}

int insert_array_ready(struct msg insert_msg)
{
    bool added = false;

    if ((shmid_job = shmget(KEY_JOB, sizeof(int), IPC_CREAT | 0x1FF)) < 0)
    {
        printf("A conexao com memoria compartilhada nao foi possivel!\n");
        exit(1);
    }
    int *job = (int *)shmat(shmid_job, (void *)0, 0);

    if (ready_queue_size == 0)
    {

        queue_matrix[READY][0].init_time = 0;
        queue_matrix[READY][0].end_time = 0;
        queue_matrix[READY][0].origin_sec = (int)insert_msg.sec;
        queue_matrix[READY][0].job = *job;
        queue_matrix[READY][0].sec = insert_msg.sec;
        strcpy(queue_matrix[READY][0].arq_executavel, insert_msg.arq_executavel);
        ready_queue_size++;
    }
    else
    {
        for (int i = 0; i <= ready_queue_size; i++)
        {
            if (queue_matrix[READY][i].sec > insert_msg.sec)
            {
                for (int j = ready_queue_size; j >= i; j--)
                {
                    queue_matrix[READY][j + 1] = queue_matrix[READY][j];
                }
                queue_matrix[READY][i].init_time = 0;
                queue_matrix[READY][i].end_time = 0;
                queue_matrix[READY][i].origin_sec = (int)insert_msg.sec;
                queue_matrix[READY][i].job = *job;
                queue_matrix[READY][i].sec = insert_msg.sec;
                strcpy(queue_matrix[READY][i].arq_executavel, insert_msg.arq_executavel);
                ready_queue_size++;
                added = true;
                break;
            }
        }

        if (!added)
        {
            queue_matrix[READY][ready_queue_size].init_time = 0;
            queue_matrix[READY][ready_queue_size].end_time = 0;
            queue_matrix[READY][ready_queue_size].origin_sec = (int)insert_msg.sec;
            queue_matrix[READY][ready_queue_size].job = *job;
            queue_matrix[READY][ready_queue_size].sec = insert_msg.sec;
            strcpy(queue_matrix[READY][ready_queue_size].arq_executavel, insert_msg.arq_executavel);
            ready_queue_size++;
        }
    }

    shmdt(job);
}

int ready_to_run()
{
    if (!ready_queue_size)
        return 0;

    for (int i = 0; i < QUEUE_SIZE; i++)
    {
        if (i > ready_queue_size)
            break;

        if (i == 0)
        {
            queue_matrix[RUN][run_queue_size] = queue_matrix[READY][i];
            queue_matrix[RUN][run_queue_size].job = queue_matrix[READY][i].job;
            run_queue_size++;
        }
        queue_matrix[READY][i] = queue_matrix[READY][i + 1];
    }
    ready_queue_size--;
}

int run_to_ended(int init_time, int end_time)
{
    if (!run_queue_size)
        return 0;

    for (int i = 0; i < QUEUE_SIZE; i++)
    {

        if (i > run_queue_size)
            break;

        if (i == 0)
        {
            queue_matrix[ENDED][ended_queue_size] = queue_matrix[RUN][i];
            queue_matrix[ENDED][ended_queue_size].init_time = init_time;
            queue_matrix[ENDED][ended_queue_size].end_time = end_time;
            ended_queue_size++;
        }
        queue_matrix[RUN][i] = queue_matrix[RUN][i + 1];
    }
    run_queue_size--;
}

#endif