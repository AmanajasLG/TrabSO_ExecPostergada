/**
 * @authors: 
 * @name Luíza Amanajás
 * @matricula 160056659
 */

#ifndef TORUS_H_

#define TORUS_H_

#include "includes.h"

void create_torus(NodoTorus torus[16])
{
    int x, y;
    int index = 0;

    torus[0].neighbor[0] = 1;
    torus[1].neighbor[0] = 2;
    torus[2].neighbor[0] = 3;
    torus[3].neighbor[0] = 7;
    torus[4].neighbor[0] = 8;
    torus[5].neighbor[0] = 4;
    torus[6].neighbor[0] = 5;
    torus[7].neighbor[0] = 6;
    torus[8].neighbor[0] = 9;
    torus[9].neighbor[0] = 10;
    torus[10].neighbor[0] = 11;
    torus[11].neighbor[0] = 15;
    torus[12].neighbor[0] = -1;
    torus[13].neighbor[0] = 12;
    torus[14].neighbor[0] = 13;
    torus[15].neighbor[0] = 14;

    torus[0].msg_rcv_number = 15;
    torus[1].msg_rcv_number = 7;
    torus[2].msg_rcv_number = 3;
    torus[3].msg_rcv_number = 3;
    torus[4].msg_rcv_number = 1;
    torus[5].msg_rcv_number = 1;
    torus[6].msg_rcv_number = 1;
    torus[7].msg_rcv_number = 1;
    torus[8].msg_rcv_number = 0;
    torus[9].msg_rcv_number = 0;
    torus[10].msg_rcv_number = 0;
    torus[11].msg_rcv_number = 0;
    torus[12].msg_rcv_number = 0;
    torus[13].msg_rcv_number = 0;
    torus[14].msg_rcv_number = 0;
    torus[15].msg_rcv_number = 0;

    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            int n[4];
            int min = 50;

            x = i;
            y = abs((j - 3) % 4);
            n[0] = x * 4 + y;

            x = i;
            y = j % 2 == 0 ? (j + 1) % 4 : abs((j - 1) % 4);
            n[1] = x * 4 + y;

            x = abs((i - 3) % 4);
            y = j;
            n[2] = x * 4 + y;

            x = i % 2 == 0 ? (i + 1) % 4 : abs((i - 1) % 4);
            y = j;
            n[3] = x * 4 + y;

            //find min neighbor
            for (int i = 0; i < 4; i++)
            {
                if (min >= n[i])
                {
                    torus[index].neighbor[1] = n[i];
                    min = n[i];
                }
            }

            index++;
        }
    }
}

void print_torus(NodoTorus torus[16])
{
    for (int i = 0; i < 16; i++)
    {
        for (int k = 0; k < 2; k++)
        {
            printf("no %d- vizinho %d: [%d]\n", i, k, torus[i].neighbor[k]);
        }

        printf("\n\n");
    }
}

void nodo_loop_torus(int my_position, NodoTorus my_nodo)
{
    signal(SIGTERM, end_node);

    struct end_msg msg_2_snd;
    struct end_msg msg_exec_end;
    struct msg_nodo msg_exec_name;
    int exec_end, exec_init;
    int msg_rcv = my_nodo.msg_rcv_number;

    msg_exec_name.id = -1;
    msg_exec_end.position = -1;

    while (1)
    {

        /* MSG DO ESCALONADOR */
        msgrcv(msgid_nodo_snd_file, &msg_exec_name, sizeof(msg_exec_name) - sizeof(long), my_position + 1, IPC_NOWAIT);

        if (msg_exec_name.id != -1)
        {
            /* MANDA PARA OS VIZINHOS */
            if (my_position != 12)
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
            pid_son_process = 0;

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

            msg_exec_name.id = -1;

            msg_rcv = my_nodo.msg_rcv_number;
        }
    }
}

void nodo_0_loop_torus(NodoTorus my_nodo)
{
    signal(SIGTERM, end_node);
    struct end_msg msg_2_snd;
    struct end_msg msg_exec_end;
    struct msg_nodo msg_exec_name;
    int exec_end, exec_init;
    int msg_rcv = my_nodo.msg_rcv_number;

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