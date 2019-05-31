#ifndef QUEUE_CONTROL_H_

#define QUEUE_CONTROL_H_

#include "includes.h"

#define QUEUE_SIZE 20

int ready_queue_size = 0;
int run_queue_size = 0;
int ended_queue_size = 0;

struct queue_nodo queue_matrix[3][QUEUE_SIZE];

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
                printf("Nodo %d(%d) -> ", queue_matrix[i][j].sec, queue_matrix[i][j].origin_sec);
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
void print_queue(Queue *queue)
{
    struct queue_nodo *tmp = queue->init;
    if (!is_empty(queue))
    {
        do
        {
            printf("Nodo %d(%d | %p) -> ", tmp->sec, tmp->origin_sec, tmp);
            tmp = tmp->next;
        } while (tmp != NULL);
    }
    else
    {
        printf("EMPTY");
    }

    printf("\n");
}

void print_nodo(struct queue_nodo *nodo)
{
    if (nodo != NULL)
    {
        printf("PRINT NODO %d(%d | %p)\n", nodo->sec, nodo->origin_sec, nodo);
    }
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

    if (run_queue->init == NULL)
    {
        run_queue->init = ready_queue->init;
        printf("ANTES RUN\n");
        print_nodo(run_queue->init);
        ready_queue->init = ready_queue->init->next;
        run_queue->init->next = NULL;
        printf("DEPOIS RUN\n");
        print_nodo(run_queue->init);
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

        previous->next = ready_queue->init;
        printf("ANTES RUN\n");
        print_nodo(previous->next);
        ready_queue->init = ready_queue->init->next;
        previous->next->next = NULL;

        printf("DEPOIS RUN\n");
        print_nodo(previous->next);

        free(tmp);
        free(previous);
    }

    return 1;
}

int insert_queue_ready(struct msg insert_msg)
{

    if (ready_queue == NULL)
        return 0;

    if (ready_queue->init == NULL)
    { //empty queue
        ready_queue->init = (struct queue_nodo *)calloc(0, sizeof(struct queue_nodo));

        printf("ANTES\n");
        print_nodo(ready_queue->init);

        ready_queue->init->end_time = 0;
        ready_queue->init->origin_sec = (int)insert_msg.sec;
        ready_queue->init->job = job;
        ready_queue->init->init_time = 0;
        ready_queue->init->sec = insert_msg.sec;
        strcpy(ready_queue->init->arq_executavel, insert_msg.arq_executavel);

        printf("DEPOIS\n");
        print_nodo(ready_queue->init);

        ready_queue->init->next = NULL;
    }
    else
    {

        struct queue_nodo *tmp = ready_queue->init;
        struct queue_nodo *previous = NULL;

        do
        {
            if (tmp->sec <= insert_msg.sec)
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
                    previous->next = (struct queue_nodo *)calloc(0, sizeof(struct queue_nodo));

                    printf("ANTES\n");
                    print_nodo(previous->next);
                    previous->next->next = aux;

                    previous->next->end_time = 0;
                    previous->next->origin_sec = (int)insert_msg.sec;
                    previous->next->job = job;
                    previous->next->init_time = 0;
                    previous->next->sec = insert_msg.sec;
                    strcpy(previous->next->arq_executavel, insert_msg.arq_executavel);

                    printf("DEPOIS\n");
                    print_nodo(previous->next);
                }
                else
                {
                    aux = ready_queue->init;
                    ready_queue->init = (struct queue_nodo *)calloc(0, sizeof(struct queue_nodo));
                    printf("ANTES\n");
                    print_nodo(ready_queue->init);
                    ready_queue->init->next = aux;
                    ready_queue->init->end_time = 0;
                    ready_queue->init->origin_sec = (int)insert_msg.sec;
                    ready_queue->init->job = job;
                    ready_queue->init->init_time = 0;
                    ready_queue->init->sec = insert_msg.sec;
                    strcpy(ready_queue->init->arq_executavel, insert_msg.arq_executavel);

                    printf("DEPOIS\n");
                    print_nodo(ready_queue->init);
                }
                break;
            }
        } while (tmp != NULL);
        if (tmp == NULL)
        {
            previous->next = (struct queue_nodo *)calloc(0, sizeof(struct queue_nodo));
            printf("ANTES\n");
            print_nodo(previous->next);
            previous->next->next = NULL;
            previous->next->end_time = 0;
            previous->next->origin_sec = (int)insert_msg.sec;
            previous->next->job = job;
            previous->next->init_time = 0;
            previous->next->sec = insert_msg.sec;
            strcpy(previous->next->arq_executavel, insert_msg.arq_executavel);

            printf("DEPOIS\n");
            print_nodo(previous->next);
        }

        free(tmp);
        free(previous);
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

    free(nodo);
    return 1;
}

int insert_array_ready(struct msg insert_msg)
{
    bool added = false;

    if (ready_queue_size == 0)
    {

        queue_matrix[READY][0].init_time = 0;
        queue_matrix[READY][0].end_time = 0;
        queue_matrix[READY][0].origin_sec = (int)insert_msg.sec;
        queue_matrix[READY][0].job = job;
        queue_matrix[READY][0].sec = insert_msg.sec;
        strcpy(queue_matrix[READY][0].arq_executavel, insert_msg.arq_executavel);
        ready_queue_size++;
    }
    else
    {

        printf("\n============QUEUE INFO Antes do for============\n");
        print_matrix();
        printf("\n==================================\n\n");
        for (int i = 0; i <= ready_queue_size; i++)
        {
            if (queue_matrix[READY][i].sec > insert_msg.sec)
            {
                printf("\n============QUEUE INFO DENTRO DO IF============\n");
                print_matrix();
                printf("\n==================================\n\n");
                for (int j = ready_queue_size; j >= i; j--)
                {
                    printf("\n============QUEUE INFO DENTRO DO FOR============\n");
                    print_matrix();
                    printf("\n==================================\n\n");
                    printf("j = %d\n", j);
                    queue_matrix[READY][j + 1] = queue_matrix[READY][j];
                }
                queue_matrix[READY][i].init_time = 0;
                queue_matrix[READY][i].end_time = 0;
                queue_matrix[READY][i].origin_sec = (int)insert_msg.sec;
                queue_matrix[READY][i].job = job;
                queue_matrix[READY][i].sec = insert_msg.sec;
                strcpy(queue_matrix[READY][i].arq_executavel, insert_msg.arq_executavel);
                ready_queue_size++;
                added = true;
                break;
            }
        }

        if (!added)
        {
            printf("\n============QUEUE INFO !ADDED============\n");
            print_matrix();
            printf("\n==================================\n\n");
            queue_matrix[READY][ready_queue_size].init_time = 0;
            queue_matrix[READY][ready_queue_size].end_time = 0;
            queue_matrix[READY][ready_queue_size].origin_sec = (int)insert_msg.sec;
            queue_matrix[READY][ready_queue_size].job = job;
            queue_matrix[READY][ready_queue_size].sec = insert_msg.sec;
            strcpy(queue_matrix[READY][ready_queue_size].arq_executavel, insert_msg.arq_executavel);
            ready_queue_size++;
        }
    }
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
            run_queue_size++;
        }
        queue_matrix[READY][i] = queue_matrix[READY][i + 1];
    }

    ready_queue_size--;
}

int run_to_ended()
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
            ended_queue_size++;
        }
        else
        {
            queue_matrix[RUN][i] = queue_matrix[RUN][i + 1];
        }
    }

    run_queue_size--;
}

#endif