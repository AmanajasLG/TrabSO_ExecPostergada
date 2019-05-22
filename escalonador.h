#ifndef ESCALONADOR_H_

#define ESCALONADOR_H_

#include "includes.h"
struct msg_nodo msg_2_nodo0;
int msgid_nodo_snd_file, pid_nodo0;
bool is_executing = false;
time_t exec_init;
Queue *ready_queue = NULL, *run_queue = NULL;


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
            printf("nodo [%d] pid %d pai: %d dir %d esq %d\n", i, fattree[i].pid, fattree[i].parent, fattree[i].right, fattree[i].left);
        break;
    }
}

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

void create_torus(NodoTorus torus[4][4])
{
    char xx[12], yy[12];
    int x, y;
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {

            x = i;
            y = abs((j - 3) % 4);

            sprintf(xx, "%d", x);
            sprintf(yy, "%d", y);

            strcat(xx, yy);
            torus[i][j].neighbor[0] = atol(xx);

            x = i;
            y = j % 2 == 0 ? (j + 1) % 4 : abs((j - 1) % 4);

            sprintf(xx, "%d", x);
            sprintf(yy, "%d", y);

            strcat(xx, yy);
            torus[i][j].neighbor[1] = atol(xx);

            x = abs((i - 3) % 4);
            y = j;

            sprintf(xx, "%d", x);
            sprintf(yy, "%d", y);

            strcat(xx, yy);
            torus[i][j].neighbor[2] = atol(xx);

            x = i % 2 == 0 ? (i + 1) % 4 : abs((i - 1) % 4);
            y = j;

            sprintf(xx, "%d", x);
            sprintf(yy, "%d", y);

            strcat(xx, yy);
            torus[i][j].neighbor[3] = atol(xx);
        }
    }
}

void print_torus(NodoTorus torus[4][4])
{
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            for (int k = 0; k < 4; k++)
            {
                printf("no [%d][%d] - vizinho %d: [%d]\n", i, j, k, torus[i][j].neighbor[k]);
            }

            printf("\n\n");
        }
    }
}

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

Queue *start_queue()
{
    Queue *ready_queue = (Queue *)malloc(sizeof(Queue));
    if (ready_queue != NULL)
    {
        ready_queue->init = NULL;
    }
    return ready_queue;
}

int insert_queue_first_pos(Queue *ready_queue, struct msg insert_msg)
{

    if (ready_queue == NULL)
        return 0;
    struct queue_nodo *new_nodo = (struct queue_nodo *)malloc(sizeof(struct queue_nodo));

    if (new_nodo == NULL)
        return 0;

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

int is_empty(Queue *ready_queue)
{
    if (ready_queue->init == NULL)
        return 1;
    else
        return 0;
}

int free_queue(Queue *ready_queue)
{
    struct queue_nodo *aux, *tmp = ready_queue->init;
    if (!is_empty(ready_queue))
    {
        do
        {
            aux = tmp;
            tmp = tmp->next;
            free(aux);

        } while (tmp != NULL);
    }
}
int remove_queue(Queue *ready_queue)
{
    if (ready_queue == NULL)
        return 0;
    if (ready_queue->init == NULL)
    {
        return 0;
    }
    struct queue_nodo *nodo = ready_queue->init;

    ready_queue->init = ready_queue->init->next;

    free(nodo);
    return 1;
}

int insert_queue(Queue *ready_queue, struct msg insert_msg)
{
    if (ready_queue == NULL)
        return 0;
    struct queue_nodo *new_nodo = (struct queue_nodo *)malloc(sizeof(struct queue_nodo));

    if (new_nodo == NULL)
        return 0;

    new_nodo->origin_sec = insert_msg.sec;
    new_nodo->sec = insert_msg.sec;
    strcpy(new_nodo->arq_executavel, insert_msg.arq_executavel);
    new_nodo->next = NULL;
    printf("New nodo %d %s\n", new_nodo->sec, new_nodo->arq_executavel);
    if (ready_queue->init == NULL)
    { //empty queue
        printf("queue init null :(\n");
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
void print_queue(Queue *ready_queue)
{
    struct queue_nodo *tmp = ready_queue->init;
    if (!is_empty(ready_queue))
    {
        do
        {
            printf("Nodo %d(%d) -> ", tmp->sec, tmp->origin_sec);
            tmp = tmp->next;
        } while (tmp != NULL);
    }

    printf("\n");
}

void manda_exec_prog()
{

    msg_2_nodo0.pid = pid_nodo0;
    if (!is_executing)
    {

        printf("msg to nodo0 [ %ld | %s ]\n", msg_2_nodo0.pid, msg_2_nodo0.arq_executavel);
        msgid_nodo_snd_file = msgget(KEY_NODO_END, 0x1FF);
        printf("\nmsg : [%ld %s]\n", msg_2_nodo0.pid, msg_2_nodo0.arq_executavel);
        msgsnd(msgid_nodo_snd_file, &msg_2_nodo0, sizeof(msg_2_nodo0) - sizeof(long), 0);
        is_executing = true;
        exec_init = time(NULL);
    }
    else
    {
        printf("erro no signal manda exec prog\n");
    }
}

void att_time(int alarm_countdown, Queue *ready_queue)
{
    int sub_time = 0;

    struct queue_nodo *tmp = ready_queue->init;

    if (tmp != NULL)
    {
        sub_time = tmp->sec - alarm_countdown;
        tmp->sec = alarm_countdown;

        if (tmp->next != NULL)
        {
            tmp = tmp->next;

            do
            {
                tmp->sec -= sub_time;
                tmp = tmp->next;
            } while (tmp != NULL);
        }
    }
}

#endif