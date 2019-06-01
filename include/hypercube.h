/**
 * @authors: 
 * @name Luíza Amanajás
 * @matricula 160056659
 */

#ifndef HYPERCUBE_H_

#define HYPERCUBE_H_

#include "includes.h"

void create_hypercube(NodoHypercube hypercube[16])
{
    hypercube[0].neighbor[0] = 2;
    hypercube[1].neighbor[0] = -1;
    hypercube[2].neighbor[0] = 10;
    hypercube[3].neighbor[0] = 11;
    hypercube[4].neighbor[0] = 5;
    hypercube[5].neighbor[0] = 13;
    hypercube[6].neighbor[0] = 4;
    hypercube[7].neighbor[0] = 3;
    hypercube[8].neighbor[0] = 12;
    hypercube[9].neighbor[0] = 1;
    hypercube[10].neighbor[0] = 8;
    hypercube[11].neighbor[0] = 9;
    hypercube[12].neighbor[0] = 14;
    hypercube[13].neighbor[0] = 15;
    hypercube[14].neighbor[0] = 6;
    hypercube[15].neighbor[0] = 7;

    hypercube[0].msg_rcv_number = 15;
    hypercube[1].msg_rcv_number = 7;
    hypercube[2].msg_rcv_number = 3;
    hypercube[3].msg_rcv_number = 3;
    hypercube[4].msg_rcv_number = 1;
    hypercube[5].msg_rcv_number = 1;
    hypercube[6].msg_rcv_number = 1;
    hypercube[7].msg_rcv_number = 1;
    hypercube[8].msg_rcv_number = 0;
    hypercube[9].msg_rcv_number = 0;
    hypercube[10].msg_rcv_number = 0;
    hypercube[11].msg_rcv_number = 0;
    hypercube[12].msg_rcv_number = 0;
    hypercube[13].msg_rcv_number = 0;
    hypercube[14].msg_rcv_number = 0;
    hypercube[15].msg_rcv_number = 0;

    int n[4];
    for (int i = 0; i < 16; i++)
    {

        n[0] = i ^ 0b0001;
        n[1] = i ^ 0b0010;
        n[2] = i ^ 0b0100;
        n[3] = i ^ 0b1000;

        int snd_end_neighbor = 50;

        for (int j = 0; j < 4; j++)
        {
            if (n[j] < snd_end_neighbor)
                snd_end_neighbor = n[j];
        }
        hypercube[i].neighbor[1] = snd_end_neighbor; // COMMITA E MERGE NA MASTER HAHAHAHAHAHAHAHAH FLWS VLWS
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

void nodo_loop_hypercube(int my_position, NodoHypercube my_nodo)
{
    signal(SIGTERM, end_node);
    struct end_msg msg_2_snd;
    struct end_msg msg_exec_end;
    struct msg_nodo msg_exec_name;
    int msg_rcv = my_nodo.msg_rcv_number;

    int exec_end, exec_init;

    msg_exec_name.id = -1;
    msg_exec_end.position = -1;

    while (1)
    {

        /* MSG DO ESCALONADOR */
        msgrcv(msgid_nodo_snd_file, &msg_exec_name, sizeof(msg_exec_name) - sizeof(long), my_position + 1, IPC_NOWAIT);

        if (msg_exec_name.id != -1)
        {
            /* MANDA PARA OS VIZINHOS */ //na 2 vez n recebeu todas as mensagens ne?
            if (my_position != 1)
            {
                msg_exec_name.id = my_nodo.neighbor[0] + 1;

                msgsnd(msgid_nodo_snd_file, &msg_exec_name, sizeof(msg_exec_name) - sizeof(long), 0);
            }
            // separa nome arq do path
            char filename[100];
            strcpy(filename, basename(msg_exec_name.arq_executavel));

            // printf("RECEBE ARQUIVO %s\n",  msg_exec_name.arq_executavel);

            //TODO ACABAR O PROGRAMA
            if ((pid_son_process = fork()) < 0)
            {
                printf("Erro no fork() -> %d\n", errno);
                continue;
            }

            exec_init = (int)time(NULL);
            if (pid_son_process == 0)
            {
                execl(msg_exec_name.arq_executavel, filename, (char *)0);
            }

            // espera no atual esperar de executar
            int state;
            wait(&state);
            exec_end = (int)time(NULL);

            // manda mensagem de volta com
            // printf("END NEIGHBOR %d NO %d\n", snd_end_neighbor, my_position);
            msg_2_snd.position = my_nodo.neighbor[1] + 1;
            msg_2_snd.end_info[0] = my_position;
            msg_2_snd.end_info[1] = exec_init;
            msg_2_snd.end_info[2] = exec_end;

            msgsnd(msgid_nodo_rcv_end, &msg_2_snd, sizeof(msg_2_snd) - sizeof(long), 0);
            while (my_position < 8)
            {
                // printf("ESPERANDO MSG FILHOS NO %d\n", my_position);
                /* MSG DOS FILHOS */
                msgrcv(msgid_nodo_rcv_end, &msg_exec_end, sizeof(msg_exec_end) - sizeof(long), my_position + 1, IPC_NOWAIT);

                if (msg_exec_end.position != -1)
                {
                    msg_exec_end.position = my_nodo.neighbor[1] + 1;
                    msgsnd(msgid_nodo_rcv_end, &msg_exec_end, sizeof(msg_exec_end) - sizeof(long), 0);

                    msg_exec_end.position = -1;

                    msg_rcv--;
                    if (msg_rcv == 0)
                        break;
                }
            }
            msg_rcv = my_nodo.msg_rcv_number;

            msg_exec_name.id = -1;
        }
    }
}

void nodo_0_loop_hypercube(NodoHypercube my_nodo)
{
    signal(SIGTERM, end_node);
    struct end_msg msg_2_snd;
    struct end_msg msg_exec_end;
    int msg_rcv = my_nodo.msg_rcv_number;

    struct msg_nodo msg_exec_name;
    int exec_end, exec_init;

    int pid;

    msg_exec_name.id = -1;
    msg_exec_end.position = -1;

    while (1)
    {

        /* MSG DO ESCALONADOR */
        msgrcv(msgid_nodo_snd_file, &msg_exec_name, sizeof(msg_exec_name) - sizeof(long), getpid(), IPC_NOWAIT);

        if (msg_exec_name.id != -1)
        {
            /* MANDA PARA OS VIZINHOS */

            msg_exec_name.id = my_nodo.neighbor[0] + 1;
            msgsnd(msgid_nodo_snd_file, &msg_exec_name, sizeof(msg_exec_name) - sizeof(long), 0);

            // separa nome arq do path
            char filename[100];
            strcpy(filename, basename(msg_exec_name.arq_executavel));

            // printf("RECEBE ARQUIVO %s\n",  msg_exec_name.arq_executavel);

            //TODO ACABAR O PROGRAMA
            if ((pid_son_process = fork()) < 0)
            {
                printf("Erro no fork() -> %d\n", errno);
                continue;
            }

            exec_init = (int)time(NULL);
            if (pid_son_process == 0)
            {
                if (execl(msg_exec_name.arq_executavel, filename, (char *)0) < 0)
                {
                    printf("Erro: execl falhou: %d\n", errno);
                }
            }

            // espera no atual esperar de executar
            int state;
            wait(&state);

            exec_end = (int)time(NULL);
            pid_son_process = 0;

            // manda mensagem de volta com
            msg_2_snd.position = getpid();
            msg_2_snd.end_info[0] = 0;
            msg_2_snd.end_info[1] = exec_init;
            msg_2_snd.end_info[2] = exec_end;

            msgsnd(msgid_nodo_rcv_end, &msg_2_snd, sizeof(msg_2_snd) - sizeof(long), 0);

            while (1)
            {
                /* MSG DOS FILHOS */
                msgrcv(msgid_nodo_rcv_end, &msg_exec_end, sizeof(msg_exec_end) - sizeof(long), 1, IPC_NOWAIT);
                if (msg_exec_end.position != -1)
                {
                    msg_exec_end.position = getpid();
                    msgsnd(msgid_nodo_rcv_end, &msg_exec_end, sizeof(msg_exec_end) - sizeof(long), 0);

                    msg_exec_end.position = -1;

                    msg_rcv--;
                    if (msg_rcv == 0)
                        break;
                }
            }
            msg_rcv = my_nodo.msg_rcv_number;

            msg_exec_name.id = -1;
        }
    }
}
#endif