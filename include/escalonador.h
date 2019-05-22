#ifndef ESCALONADOR_H_

#define ESCALONADOR_H_

#include "../include/torus.h"
#include "../include/hypercube.h"
#include "../include/tree.h"

struct msg_nodo msg_2_nodo0;
int msgid_nodo_snd_file, pid_nodo0;
bool is_executing = false;
time_t exec_init;
Queue *ready_queue = NULL, *run_queue = NULL;

/* QUEUE CONTROLLER */
void print_topology(int type, TreeNodo *fattree)
{
    switch (type)
    {
    case HYPERCUBE:
        break;
    case TORUS:
        break;
    case TREE:
        for (int i = 0; i < 15; i++)
            printf("nodo [%d] pid %d pai: %d dir %d esq %d\n", i, fattree[i].pid, fattree[i].parent, fattree[i].right, fattree[i].left);
        break;
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

/* ATUALIZA TIME DOS PROGS ESPERANDO NA FILA READY */
void att_time(int alarm_countdown, Queue *ready_queue)
{
    int sub_time = 0;

    struct queue_nodo *tmp = ready_queue->init;

    if (tmp != NULL)
    {
        sub_time = tmp->sec - alarm_countdown;
        tmp->sec = alarm_countdown;

        if ((tmp = tmp->next) != NULL)
        {
            do
            {
                tmp->sec -= sub_time;
                tmp = tmp->next;
            } while (tmp != NULL);
        }
    }
}

/* MANDA MSG PARA O NÓ ZERO COM O PROG A SER EXEC */
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
        *all_ended = false;
    }
    else
    {
        printf("erro no signal manda exec prog\n");
    }
}

/* LOOP COM AS FUNCIONALIDADES DO ESCALONADOR */
void loop_escalonator(int msgid_escale, int msgid_nodo_rcv_end, int shmid_all_ended, int count_end_origin)
{
    /* SETA FUNÇÃO QUE SERÁ EXEC QUANDO RECEBER O ALARM */
    signal(SIGALRM, manda_exec_prog);

    int alarm_countdown, count_end = count_end_origin;
    struct msg msg_from_exec_post;
    struct end_msg msg_from_nodo0;
    all_ended = (bool *)shmat(shmid_all_ended, (char *)0, 0);
    *all_ended = false;

    /* ESPERA A PRIMEIRA MGS BLOQ PQ SE AINDA N RECEBEU NENHUMA NÃO TEM O QUE FAZER */
    msgrcv(msgid_escale, &msg_from_exec_post, sizeof(msg_from_exec_post) - sizeof(long), 0, 0);
    insert_queue(ready_queue, msg_from_exec_post);
    // print_queue(ready_queue);
    strcpy(msg_2_nodo0.arq_executavel, msg_from_exec_post.arq_executavel);
    alarm((int)msg_from_exec_post.sec);

    /* SETA LONG -1 PARA MOSTRAR QUE N TEM MSG NOVA (PRIMEIRA MSG JÁ TRATADA ACIMA) */
    msg_from_exec_post.sec = -1;
    msg_from_nodo0.position = -1;
    while (1)
    {
        /* ESPERA MSG DO EXEC POST OU DO NÓ 0 INFORMANDO QUE ALGUM NÓ ACABOU DE EXEC */
        msgrcv(msgid_escale, &msg_from_exec_post, sizeof(msg_from_exec_post) - sizeof(long), 0, IPC_NOWAIT);
        msgrcv(msgid_nodo_rcv_end, &msg_from_nodo0, sizeof(msg_from_nodo0) - sizeof(long), pid_nodo0, IPC_NOWAIT);

        // -1 significa que n chegou mensagem
        if (msg_from_exec_post.sec != -1)
        {
            alarm_countdown = alarm(0);
            att_time(alarm_countdown, ready_queue);
            // print_queue(ready_queue);
            if (msg_from_exec_post.sec < alarm_countdown)
            {
                insert_queue_first_pos(ready_queue, msg_from_exec_post);
                // print_queue(ready_queue);
                alarm(msg_from_exec_post.sec);
            }
            else
            {
                insert_queue(ready_queue, msg_from_exec_post);
                // print_queue(ready_queue);
                alarm(alarm_countdown);
            }
            msg_from_exec_post.sec = -1;
        }

        if (msg_from_nodo0.position != -1)
        {
            count_end--;

            printf("No %d terminou:\n", msg_from_nodo0.end_info[0]);
            printf("Hora de inicio: %d\n", msg_from_nodo0.end_info[1]);
            printf("Hora de termino: %d\n", msg_from_nodo0.end_info[2]);

            if (count_end == 0)
            {
                is_executing = false;

                while (msg_from_nodo0.position != -1)
                {
                    msg_from_nodo0.position = -1;
                    msgrcv(msgid_nodo_rcv_end, &msg_from_nodo0, sizeof(msg_from_nodo0) - sizeof(long), 0, IPC_NOWAIT);
                }

                if (!is_empty(run_queue))
                {
                    *all_ended = true;
                    strcpy(msg_2_nodo0.arq_executavel, run_queue->init->arq_executavel);
                    manda_exec_prog();
                }
                count_end = count_end_origin;
            }
            // printf("Tempo total de exec: %d s", exec_init - exec_end);

            msg_from_nodo0.position = -1;
        }
    }
}

#endif