#ifndef ESCALONADOR_H_

#define ESCALONADOR_H_

#include "../include/torus.h"
#include "../include/hypercube.h"
#include "../include/tree.h"
#include "../include/queue_control.h"

struct msg_nodo msg_2_nodo0;
int msgid_nodo_snd_file, pid_nodo0;
bool is_executing = false;

/* QUANDO PROG TERMINA LIBERA TUDO */
void end_program(int msgid_escale, int msgid_nodo_rcv_end, int shmid_all_ended){
    printf("DESTROY ALL\n");
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
            remove_queue_ready();
            insert_queue_run();
        }

        printf("msg to nodo0 [ %ld | %s ]\n", msg_2_nodo0.pid, msg_2_nodo0.arq_executavel);

        msgsnd(msgid_nodo_snd_file, &msg_2_nodo0, sizeof(msg_2_nodo0) - sizeof(long), 0);

        is_executing = true;
        *all_ended = 0;
    }
    else
    {
        att_time(get_first_sec());

        /* REMOVE DA LISTA READY PARA A RUN */
        remove_queue_ready();
        insert_queue_run();
    }

    /* SETA O PROX ALARM */
    alarm(get_first_sec());

    printf("\nQUEUES SEND\n");
    print_queue(ready_queue);
    print_queue(run_queue);
    print_queue(ended_queue);
    printf("\n\n");
}

/* LOOP COM AS FUNCIONALIDADES DO ESCALONADOR */
void loop_escalonator(int msgid_escale, int msgid_nodo_rcv_end, int shmid_all_ended, int count_end_origin)
{
    printf("go\n");
    msg_2_nodo0.pid = pid_nodo0;
    msgid_nodo_snd_file = msgget(KEY_NODO_END, 0x1FF);

    /* SETA FUNÇÃO QUE SERÁ EXEC QUANDO RECEBER O ALARM */
    signal(SIGALRM, manda_exec_prog);

    int alarm_countdown, count_end = count_end_origin;
    struct msg msg_from_exec_post;
    struct end_msg msg_from_nodo0;
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
                alarm(alarm_countdown);
            }
            msg_from_exec_post.sec = -1;
        }

        if (msg_from_nodo0.position != -1)
        {
            count_end--;

            char init[30], end[30];

            printf("No %d terminou:\n", msg_from_nodo0.end_info[0]);
            time_t init_time = (time_t)msg_from_nodo0.end_info[1];
            struct tm *tm_init = localtime(&init_time);
            strftime(init, 30, "%d/%m/%Y, %H:%M:%S", tm_init);
            printf("Hora de inicio: %s\n", init);
            time_t end_time = (time_t)msg_from_nodo0.end_info[2];
            struct tm *tm_end = localtime(&end_time);
            strftime(end, 30, "%d/%m/%Y, %H:%M:%S", tm_end);
            printf("Hora de termino: %s\n\n", end);

            if (count_end == 0)
            {
                is_executing = false;
                printf("TERMINANDO EXECUCAO DO JOB %d - %s\n\n", run_queue->init->job, run_queue->init->arq_executavel);
                remove_queue_run();
                insert_queue_ended();

                printf("\nQUEUES END\n");
                print_queue(ready_queue);
                print_queue(run_queue);
                print_queue(ended_queue);
                printf("\n\n");

                //LIMPA FILA
                printf("ANTES DE ENTRAR\n");
                FILE* open = popen("ipcs -q", "r");
                if (open == NULL)
                {
                    perror("Error while opening the file.\n");
                    exit(EXIT_FAILURE);
                }
                
                char ch;
                while((ch = fgetc(open)) != EOF)
                    printf("%c", ch);

                do
                {
                    printf("ENTROU\n");
                    msg_from_nodo0.position = -1;
                    msgrcv(msgid_nodo_snd_file, &msg_from_nodo0, sizeof(msg_from_nodo0) - sizeof(long), 0, IPC_NOWAIT);
                    if( msg_from_nodo0.position != -1){
                        printf("DELETANDO MSG\n\n");
                    }
                } while (msg_from_nodo0.position != -1);

                if (!is_empty(run_queue))
                {
                    manda_exec_prog();
                }
                count_end = count_end_origin;
                *all_ended = 1;
            }

            msg_from_nodo0.position = -1;
        }
    }

    shmdt((char *)0);
}

#endif