#ifndef QUEUE_CONTROL_H_

#define QUEUE_CONTROL_H_

#include "includes.h"

Queue *ready_queue = NULL, *run_queue = NULL, *ended_queue = NULL;

Queue *start_queue()
{
    Queue *queue = (Queue *)malloc(sizeof(Queue));
    if (queue != NULL)
    {
        queue->init = NULL;
    }
    return queue;
}

int is_empty(Queue *queue)
{
    if (queue->init == NULL)
        return 1;
    else
        return 0;
}

void print_queue(Queue *queue)
{
    struct queue_nodo *tmp = queue->init;
    if (!is_empty(queue))
    {
        do
        {
            printf("Nodo %d(%d) -> ", tmp->sec, tmp->origin_sec);
            tmp = tmp->next;
        } while (tmp != NULL);
    }
    else
    {
        printf("EMPTY");
    }

    printf("\n");
}

int free_queue(Queue *queue)
{
    struct queue_nodo *aux, *tmp = queue->init;
    if (!is_empty(queue))
    {
        do
        {
            aux = tmp;
            tmp = tmp->next;
            free(aux);

        } while (tmp != NULL);
    }
    return 1;
}

/* READY QUEUE CONTROLLER */
int insert_queue_ready_first_pos(struct msg insert_msg)
{

    if (ready_queue == NULL)
        return 0;
    struct queue_nodo *new_nodo = (struct queue_nodo *)malloc(sizeof(struct queue_nodo));

    if (new_nodo == NULL)
        return 0;

    new_nodo->end_time = 0;
    new_nodo->init_time = 0;
    new_nodo->job = job;
    new_nodo->sec = insert_msg.sec;
    new_nodo->origin_sec = insert_msg.sec;
    strcpy(new_nodo->arq_executavel, insert_msg.arq_executavel);
    new_nodo->next = NULL;

    if (ready_queue->init == NULL)
    {
        ready_queue->init = new_nodo;
    }
    else
    {
        struct queue_nodo *second_nodo = ready_queue->init;
        ready_queue->init = new_nodo;
        new_nodo->next = second_nodo;
    }

    return 1;
}

int from_ready_to_run()
{

    if (ready_queue == NULL)
        return 0;

    if (ready_queue->init == NULL)
    {
        return 0;
    }

    if (run_queue == NULL)
        return 0;

    struct queue_nodo *new_nodo = (struct queue_nodo *)malloc(sizeof(struct queue_nodo));

    new_nodo->end_time = 0;
    new_nodo->init_time = 0;
    new_nodo->job = ready_queue->init->job;
    new_nodo->origin_sec = ready_queue->init->origin_sec;
    new_nodo->sec = ready_queue->init->sec;
    strcpy(new_nodo->arq_executavel, ready_queue->init->arq_executavel);
    new_nodo->next = NULL;

    if (run_queue->init == NULL)
    {
        run_queue->init = new_nodo;
    }
    else
    {
        struct queue_nodo *tmp = run_queue->init;
        struct queue_nodo *previous = NULL;

        while (tmp != NULL)
        {
            previous = tmp;
            tmp = tmp->next;
        }

        previous->next = new_nodo;

        free(tmp);
        free(previous);
    }

    struct queue_nodo *nodo = ready_queue->init;
    ready_queue->init = ready_queue->init->next;

    free(nodo);

    return 1;
}

int insert_queue_ready(struct msg insert_msg)
{
    if (ready_queue == NULL)
        return 0;
    struct queue_nodo *new_nodo = (struct queue_nodo *)malloc(sizeof(struct queue_nodo));

    if (new_nodo == NULL)
        return 0;

    new_nodo->end_time = 0;
    new_nodo->init_time = 0;
    new_nodo->job = job;
    new_nodo->origin_sec = insert_msg.sec;
    new_nodo->sec = insert_msg.sec;
    strcpy(new_nodo->arq_executavel, insert_msg.arq_executavel);
    new_nodo->next = NULL;
    if (ready_queue->init == NULL)
    { //empty queue
        ready_queue->init = new_nodo;
    }
    else
    {
        struct queue_nodo *tmp = ready_queue->init;
        struct queue_nodo *previous = NULL;

        do
        {
            if (tmp->sec <= new_nodo->sec)
            {
                previous = tmp;
                tmp = tmp->next;
            }
            else
            {
                struct queue_nodo *aux = NULL;
                if (previous != NULL)
                {
                    aux = previous->next;
                    previous->next = new_nodo;
                    new_nodo->next = aux;
                }
                else
                {
                    new_nodo->next = ready_queue->init;
                    ready_queue->init = new_nodo;
                }
                break;
            }
        } while (tmp != NULL);
        if (tmp == NULL)
        {
            previous->next = new_nodo;
            new_nodo->next = NULL;
        }
    }

    return 1;
}

/* RUN QUEUE CONTROLLER */
int from_run_to_ended(time_t init_time, time_t end_time)
{
    if (run_queue == NULL)
        return 0;
    if (run_queue->init == NULL)
    {
        return 0;
    }

    if (ended_queue == NULL)
        return 0;

    struct queue_nodo *new_nodo = (struct queue_nodo *)malloc(sizeof(struct queue_nodo));

    new_nodo->job = run_queue->init->job;
    new_nodo->origin_sec = run_queue->init->origin_sec;
    new_nodo->sec = run_queue->init->sec;
    new_nodo->end_time = end_time;
    new_nodo->init_time = init_time;
    strcpy(new_nodo->arq_executavel, run_queue->init->arq_executavel);
    new_nodo->next = NULL;

    if (ended_queue->init == NULL)
    { //empty queue
        ended_queue->init = new_nodo;
    }
    else
    {
        struct queue_nodo *tmp = ended_queue->init;
        struct queue_nodo *previous = NULL;

        while (tmp != NULL)
        {
            previous = tmp;
            tmp = tmp->next;
        }

        previous->next = new_nodo;
    }

    struct queue_nodo *nodo = run_queue->init;
    run_queue->init = run_queue->init->next;

    return 1;
}

int insert_queue_run()
{
}

/* ENDED QUEUE CONTROLLER */
int insert_queue_ended(time_t init_time, time_t end_time)
{

    return 1;
}

#endif