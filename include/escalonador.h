#ifndef ESCALONADOR_H_

#define ESCALONADOR_H_

#include "../include/torus.h"
#include "../include/hypercube.h"
#include "../include/tree.h"
#include "../include/queue_control.h"

int time_init;

struct msg_nodo msg_2_nodo0;
int msgid_nodo_snd_file, pid_nodo0, msgid_escale, msgid_nodo_rcv_end, shmid_all_ended;
bool is_executing = false;

/* QUANDO PROG TERMINA LIBERA TUDO */
void end_program()
{
    int status;
    printf("Removing topologies, queues and shared memories...\n");
    for (int i = 0; i < 16; i++)
    {
        if (topology == 2 && i == 15)
            break;

        if (topology == 0)
        {
            kill(hypercube[i].pid, SIGKILL);
            wait(&status);
        }
        else if (topology == 1)
        {
            kill(torus[i].pid, SIGKILL);
            wait(&status);
        }
        else
        {
            kill(tree[i].pid, SIGKILL);
            wait(&status);
        }
    }

    struct queue_nodo *aux_nodo = (struct queue_nodo *)malloc(sizeof(struct queue_nodo));
    printf("\nO ESCALONADOR SERA INTERROMPIDO!\nOS PROGRAMAS ABAIXO NAO SERAO EXECUTADOS:\n\n");
    aux_nodo = ready_queue->init;

    while (aux_nodo != NULL)
    {
        printf("JOB: %d FILE: %s\n", aux_nodo->job, aux_nodo->arq_executavel);
        aux_nodo = aux_nodo->next;
    }

    aux_nodo = run_queue->init;

    while (aux_nodo != NULL)
    {
        printf("JOB: %d FILE: %s\n", aux_nodo->job, aux_nodo->arq_executavel);
        aux_nodo = aux_nodo->next;
    }

    printf("\n\n");
    printf("OS PROGRAMAS A SEGUIR FORAM EXECUTADOS:\n\n");

    aux_nodo = ended_queue->init;

    char init[30], end[30];

    while (aux_nodo != NULL)
    {
        struct tm *tm_init = localtime(&aux_nodo->init_time);
        strftime(init, 30, "%d/%m/%Y, %H:%M:%S", tm_init);
        struct tm *tm_end = localtime(&aux_nodo->end_time);
        strftime(end, 30, "%d/%m/%Y, %H:%M:%S", tm_end);

        printf("JOB: %d FILE: %s SUBMISSON_TIME: %d INIT_TIME: [%s] END_TIME: [%s] MAKESPAN: %ld\n", aux_nodo->job, aux_nodo->arq_executavel, aux_nodo->origin_sec, init, end, aux_nodo->end_time - aux_nodo->init_time);
        aux_nodo = aux_nodo->next;
    }
    /* DESTROI FILAS E LISTAS */
    struct msqid_ds *msqbuf = malloc(sizeof(struct msqid_ds));
    struct shmid_ds *shmbuf = malloc(sizeof(struct shmid_ds));
    msgctl(msgid_escale, IPC_RMID, msqbuf);
    msgctl(msgid_nodo_snd_file, IPC_RMID, msqbuf);
    msgctl(msgid_nodo_rcv_end, IPC_RMID, msqbuf);
    shmctl(shmid_all_ended, IPC_RMID, shmbuf);
    free_queue(ready_queue);
    free_queue(run_queue);
    free_queue(ended_queue);

    exit(0);
}

/* ATUALIZA TIME DOS PROGS ESPERANDO NA FILA READY */
void att_time(int alarm_countdown)
{

    int sub_time = 0;

    struct queue_nodo *tmp = ready_queue->init;

    if (tmp != NULL)
    {
        if (tmp->sec == alarm_countdown)
        {
            sub_time = tmp->sec;
            tmp->sec = 0;
        }
        else
        {
            sub_time = tmp->sec - alarm_countdown;
            tmp->sec = alarm_countdown;
        }

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

int get_first_sec()
{
    return is_empty(ready_queue) ? 0 : ready_queue->init->sec;
}

/* MANDA MSG PARA O NÓ ZERO COM O PROG A SER EXEC */
void manda_exec_prog()
{

    if (!is_executing)
    {
        if (!is_empty(run_queue))
        {
            strcpy(msg_2_nodo0.arq_executavel, run_queue->init->arq_executavel);
            printf("INICIANDO EXECUCAO DO JOB %d - %s\n\n", run_queue->init->job, run_queue->init->arq_executavel);
        }
        else
        {
            att_time(get_first_sec());
            printf("INICIANDO EXECUCAO DO JOB %d - %s\n\n", ready_queue->init->job, ready_queue->init->arq_executavel);
            /* REMOVE DA LISTA READY PARA A RUN */
            from_ready_to_run();
        }

        // printf("msg to nodo0 [ %ld | %s ]\n", msg_2_nodo0.pid, msg_2_nodo0.arq_executavel);

        is_executing = true;
        *all_ended = 0;

        msgsnd(msgid_nodo_snd_file, &msg_2_nodo0, sizeof(msg_2_nodo0) - sizeof(long), 0);
        time_init = (int)time(NULL);
    }
    else
    {
        att_time(get_first_sec());

        /* REMOVE DA LISTA READY PARA A RUN */
        from_ready_to_run();
    }

    /* SETA O PROX ALARM */
    alarm(get_first_sec());

    printf("\n============QUEUE INFO============\n");
    printf("READY: ");
    print_queue(ready_queue);
    printf("RUN: ");
    print_queue(run_queue);
    printf("ENDED: ");
    print_queue(ended_queue);
    printf("\n==================================\n\n");
}

/* LOOP COM AS FUNCIONALIDADES DO ESCALONADOR */
void loop_escalonator(int msgid_escale, int msgid_nodo_rcv_end, int shmid_all_ended, int count_end_origin)
{
    printf("Escalonator Ready for execution\n");
    msg_2_nodo0.pid = pid_nodo0;
    msgid_nodo_snd_file = msgget(KEY_NODO_FILE, 0x1FF);

    /* SETA FUNÇÃO QUE SERÁ EXEC QUANDO RECEBER O ALARM */
    signal(SIGALRM, manda_exec_prog);

    int alarm_countdown, count_end = count_end_origin;
    struct msg msg_from_exec_post;
    struct end_msg msg_from_nodo0;
    struct msg_all_ended msg_all_ended;
    all_ended = (int *)shmat(shmid_all_ended, (char *)0, 0);
    *all_ended = 0;

    /* ESPERA A PRIMEIRA MGS BLOQ PQ SE AINDA N RECEBEU NENHUMA NÃO TEM O QUE FAZER */
    msgrcv(msgid_escale, &msg_from_exec_post, sizeof(msg_from_exec_post) - sizeof(long), 0, 0);
    insert_queue_ready(msg_from_exec_post);
    job++;
    // print_queue(ready_queue);
    strcpy(msg_2_nodo0.arq_executavel, msg_from_exec_post.arq_executavel);
    alarm((int)msg_from_exec_post.sec);

    /* SETA LONG -1 PARA MOSTRAR QUE N TEM MSG NOVA (PRIMEIRA MSG JÁ TRATADA ACIMA) */
    msg_from_exec_post.sec = -1;
    msg_from_nodo0.position = -1;
    /* SETA ALL_ENDED INFO */
    msg_all_ended.id = ALL_ENDED_DELTA;
    msg_all_ended.all_ended = true;
    while (1)
    {
        /* ESPERA MSG DO EXEC POST OU DO NÓ 0 INFORMANDO QUE ALGUM NÓ ACABOU DE EXEC */
        msgrcv(msgid_escale, &msg_from_exec_post, sizeof(msg_from_exec_post) - sizeof(long), 0, IPC_NOWAIT);
        msgrcv(msgid_nodo_rcv_end, &msg_from_nodo0, sizeof(msg_from_nodo0) - sizeof(long), pid_nodo0, IPC_NOWAIT);

        // -1 significa que n chegou mensagem
        if (msg_from_exec_post.sec != -1)
        {
            alarm_countdown = alarm(0);
            att_time(alarm_countdown);
            // print_queue(ready_queue);
            if (msg_from_exec_post.sec < alarm_countdown)
            {
                insert_queue_ready_first_pos(msg_from_exec_post);
                job++;
                alarm(msg_from_exec_post.sec);
            }
            else
            {
                insert_queue_ready(msg_from_exec_post);
                job++;
                if (alarm_countdown == 0 && ready_queue->init->sec == 0)
                {
                    manda_exec_prog();
                }
                else if (alarm_countdown == 0)
                {
                    alarm(ready_queue->init->sec);
                }
                else
                {
                    alarm(alarm_countdown);
                }
            }
            msg_from_exec_post.sec = -1;
        }

        if (msg_from_nodo0.position != -1)
        {

            count_end--;

            if (count_end == 0)
            {
                is_executing = false;

                char init[30], end[30];

                printf("TERMINANDO EXECUCAO DO JOB %d - %s\n\n", run_queue->init->job, run_queue->init->arq_executavel);
                time_t init_time = (time_t)time_init;
                struct tm *tm_init = localtime(&init_time);
                strftime(init, 30, "%d/%m/%Y, %H:%M:%S", tm_init);
                printf("Hora de inicio: %s\n", init);
                time_t end_time = (time_t)msg_from_nodo0.end_info[2];
                struct tm *tm_end = localtime(&end_time);
                strftime(end, 30, "%d/%m/%Y, %H:%M:%S", tm_end);
                printf("Hora de termino: %s\n\n", end);

                from_run_to_ended(init_time, end_time);

                printf("\n============QUEUE INFO============\n");
                printf("READY: ");
                print_queue(ready_queue);
                printf("RUN: ");
                print_queue(run_queue);
                printf("ENDED: ");
                print_queue(ended_queue);
                printf("\n==================================\n\n");

                //LIMPA FILA
                do
                {
                    msg_2_nodo0.pid = -1;
                    msgrcv(msgid_nodo_snd_file, &msg_2_nodo0, sizeof(msg_2_nodo0) - sizeof(long), 0, IPC_NOWAIT);
                } while (msg_2_nodo0.pid != -1);

                msg_2_nodo0.pid = pid_nodo0;
                count_end = count_end_origin;
                msgsnd(msgid_nodo_snd_file, &msg_all_ended, sizeof(&msg_all_ended) - sizeof(long), IPC_NOWAIT);
                printf("MSG ENVIADA ALL_ENDED!!\n");

                msg_all_ended.id = -1;
                msgrcv(msgid_nodo_rcv_end, &msg_all_ended, sizeof(msg_all_ended) - sizeof(long), pid_nodo0, 0);
                printf("NAO BOLOQUEOU E A MSG FOI: %ld", msg_all_ended.id);
                if (!is_empty(run_queue))
                {
                    manda_exec_prog();
                }
            }
            msg_from_nodo0.position = -1;
        }
    }
}

#endif