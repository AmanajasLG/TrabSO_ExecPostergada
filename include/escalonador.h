/**
 * @authors: 
 * @name Luíza Amanajás
 * @matricula 160056659
 * @name Yuri Castro do Amaral
 * @matricula 140033718
 */

#ifndef ESCALONADOR_H_

#define ESCALONADOR_H_

#include "../include/torus.h"
#include "../include/hypercube.h"
#include "../include/tree.h"
#include "../include/queue_control.h"

struct msg_nodo msg_2_nodo0;
int pid_nodo0;
bool is_executing = false;
int *job;

/* QUANDO PROG TERMINA LIBERA TUDO */
void end_program()
{
    alarm(0);
    int status;
    printf("Removendo topologia, filas e memoria compartilhada...\n");
    for (int i = 0; i < 16; i++)
    {
        if (topology == 2 && i == 15)
            break;

        if (topology == 0)
        {
            kill(hypercube[i].pid, SIGTERM);
            wait(&status);
        }
        else if (topology == 1)
        {
            kill(torus[i].pid, SIGTERM);
            wait(&status);
        }
        else
        {
            kill(tree[i].pid, SIGTERM);
            wait(&status);
        }
    }

    msgctl(msgid_escale, IPC_RMID, NULL);
    msgctl(msgid_nodo_snd_file, IPC_RMID, NULL);
    msgctl(msgid_nodo_rcv_end, IPC_RMID, NULL);
    shmdt(job);
    shmctl(shmid_job, IPC_RMID, NULL);

    char init[30], end[30];

    for (int i = 0; i < 3; i++)
    {
        if (i == ENDED)
        {
            printf("\nOS PROGRAMAS A SEGUIR FORAM EXECUTADOS:\n\n");
        }
        else if (i == READY)
        {
            printf("\nO ESCALONADOR SERA INTERROMPIDO!\nOS PROGRAMAS ABAIXO NAO SERAO EXECUTADOS:\n\n");
        }

        for (int j = 0; j < QUEUE_SIZE; j++)
        {

            if (i == ENDED && j < ended_queue_size)
            {
                struct tm *tm_init = localtime(&queue_matrix[i][j].init_time);
                strftime(init, 30, "%d/%m/%Y, %H:%M:%S", tm_init);
                struct tm *tm_end = localtime(&queue_matrix[i][j].end_time);
                strftime(end, 30, "%d/%m/%Y, %H:%M:%S", tm_end);

                printf("JOB: %d ARQ: %s DELAY: %d INI_TIME: [%s] TERM_TIME: [%s] MAKESPAN: %ld\n", queue_matrix[i][j].job, queue_matrix[i][j].arq_executavel, queue_matrix[i][j].origin_sec, init, end, queue_matrix[i][j].end_time - queue_matrix[i][j].init_time);
            }
            else if ((i == READY && j < ready_queue_size) || (i == RUN && j < run_queue_size))
            {
                printf("JOB: %d ARQ: %s\n", queue_matrix[i][j].job, queue_matrix[i][j].arq_executavel);
            }
            else
            {
                break;
            }
        }
    }

    exit(0);
}

/* ATUALIZA TIME DOS PROGS ESPERANDO NA FILA READY */
void att_time_matrix(int alarm_countdown)
{
    int sub_time = 0;

    if (ready_queue_size == 0)
        return;

    for (int i = 0; i < ready_queue_size; i++)
    {
        if (i == 0)
        {
            if (queue_matrix[READY][i].sec == 0)
            {
                sub_time = 0;
                ready_to_run();
            }
            else if (queue_matrix[READY][i].sec == alarm_countdown)
            {
                sub_time = queue_matrix[READY][i].sec;
                queue_matrix[READY][i].sec = 0;
            }
            else if (queue_matrix[READY][i].sec < alarm_countdown)
            {
                sub_time = queue_matrix[READY][i].sec;

            }
            else
            {
                sub_time = queue_matrix[READY][i].sec - alarm_countdown;
                queue_matrix[READY][i].sec = alarm_countdown;
            }
        }
        else
        {
            queue_matrix[READY][i].sec -= sub_time;
        }
    }
}

int get_first_sec()
{
    return ready_queue_size == 0 ? 0 : queue_matrix[READY][0].sec;
}

/* MANDA MSG PARA O NÓ ZERO COM O PROG A SER EXEC */
void manda_exec_prog()
{

    if (!is_executing)
    {
        if (run_queue_size != 0)
        {
            strcpy(msg_2_nodo0.arq_executavel, queue_matrix[RUN][0].arq_executavel);
            printf("INICIANDO EXECUCAO DO JOB %d - %s\n\n", queue_matrix[RUN][0].job, queue_matrix[RUN][0].arq_executavel);
        }
        else
        {
            att_time_matrix(get_first_sec());
            printf("INICIANDO EXECUCAO DO JOB %d - %s\n\n", queue_matrix[READY][0].job, queue_matrix[READY][0].arq_executavel);
            /* REMOVE DA LISTA READY PARA A RUN */
            for (int i = 0; i < ready_queue_size; i++)
            {
                if (queue_matrix[READY][i].sec > 0)
                    break;

                ready_to_run();
            }
            /* SETA O PROX ALARM */
            alarm(get_first_sec());
        }

        printf("msg to nodo0 [ %ld | %s ]\n", msg_2_nodo0.id, msg_2_nodo0.arq_executavel);

        is_executing = true;

        msgsnd(msgid_nodo_snd_file, &msg_2_nodo0, sizeof(msg_2_nodo0) - sizeof(long), 0);
    }
    else
    {
        att_time_matrix(get_first_sec());

        /* REMOVE DA LISTA READY PARA A RUN */

        for (int i = 0; i < ready_queue_size; i++)
        {
            if (queue_matrix[READY][i].sec > 0)
                break;

            ready_to_run();
        }

        /* SETA O PROX ALARM */
        alarm(get_first_sec());
    }

    printf("\n============QUEUE INFO============\n");
    print_matrix();
    printf("\n==================================\n\n");
}

/* LOOP COM AS FUNCIONALIDADES DO ESCALONADOR */
void loop_escalonator(int count_end_origin)
{
    if ((shmid_job = shmget(KEY_JOB, sizeof(int), IPC_CREAT | 0x1FF)) < 0)
    {
        printf("A conexao com memoria compartilhada nao foi possivel!\n");
        exit(1);
    }
    job = (int *)shmat(shmid_job, (void *)0, 0);

    signal(SIGTERM, end_program);

    struct msg msg_from_exec_post;
    struct end_msg msg_from_nodo0;

    msg_2_nodo0.id = pid_nodo0;
    msgid_nodo_snd_file = msgget(KEY_NODO_FILE, 0x1FF);
    int time_init, time_end;

    /* SETA FUNÇÃO QUE SERÁ EXEC QUANDO RECEBER O ALARM */
    signal(SIGALRM, manda_exec_prog);

    int alarm_countdown, count_end = count_end_origin;

    printf("Escalonador pronto para execucao!\n");

    /* ESPERA A PRIMEIRA MGS BLOQ PQ SE AINDA N RECEBEU NENHUMA NÃO TEM O QUE FAZER */
    msgrcv(msgid_escale, &msg_from_exec_post, sizeof(msg_from_exec_post) - sizeof(long), 0, 0);
    printf("Recebendo mensagem primeiro exec post sec [%ld] file [%s]\n", msg_from_exec_post.sec, msg_from_exec_post.arq_executavel);
    insert_array_ready(msg_from_exec_post);
    *job += 1;
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
            printf("Recebendo mensagem exec post job [%d] sec [%ld] file [%s]\n", *job, msg_from_exec_post.sec, msg_from_exec_post.arq_executavel);
            alarm_countdown = alarm(0);
            att_time_matrix(alarm_countdown);

            insert_array_ready(msg_from_exec_post);
            *job += 1;
            if (alarm_countdown == 0 && queue_matrix[READY][0].sec == 0)
            { // aqui ele testa se é zero e chama a func
                manda_exec_prog();
                alarm(queue_matrix[READY][0].sec);
            }
            else if (alarm_countdown == 0)
            {
                alarm(queue_matrix[READY][0].sec);
            }
            else
            {
                alarm(alarm_countdown);
            }

            msg_from_exec_post.sec = -1;
        }

        if (msg_from_nodo0.position != -1)
        {

            if (count_end == count_end_origin)
            {
                time_init = msg_from_nodo0.end_info[1];
                time_end = msg_from_nodo0.end_info[2];
            }
            else if (msg_from_nodo0.end_info[1] < time_init)
            {
                time_init = msg_from_nodo0.end_info[1];
            }

            if (msg_from_nodo0.end_info[2] < time_end)
            {
                time_end = msg_from_nodo0.end_info[2];
            }

            count_end--;

            if (count_end == 0)
            {
                char init[30], end[30];
                
                printf("TERMINANDO EXECUCAO DO JOB %d - %s\n\n", queue_matrix[RUN][0].job, queue_matrix[RUN][0].arq_executavel);
                
                time_t init_time = (time_t)time_init;
                struct tm *tm_init = localtime(&init_time);
                strftime(init, 30, "%d/%m/%Y, %H:%M:%S", tm_init);
                printf("Hora de inicio: %s\n", init);
                time_t end_time = (time_t)time_end;
                struct tm *tm_end = localtime(&end_time);
                strftime(end, 30, "%d/%m/%Y, %H:%M:%S", tm_end);
                printf("Hora de termino: %s\n\n", end);

                run_to_ended(init_time, end_time);

                msg_2_nodo0.id = pid_nodo0;
                count_end = count_end_origin;
                is_executing = false;

               

                if ((alarm_countdown = alarm(0)) == 0)
                {
                    att_time_matrix(alarm_countdown);
                    alarm(queue_matrix[READY][0].sec);
                }
                else
                {
                    att_time_matrix(alarm_countdown);
                    alarm(alarm_countdown);
                }

                if (run_queue_size > 0 || (queue_matrix[READY][0].sec == 0 && ready_queue_size > 0))
                {
                    manda_exec_prog();
                }
            }
            msg_from_nodo0.position = -1;
        }
    }
}

#endif

