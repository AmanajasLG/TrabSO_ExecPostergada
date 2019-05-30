#ifndef HYPERCUBE_H_

#define HYPERCUBE_H_

#include "includes.h"

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

void nodo_loop_hypercube(int msgid_nodo_snd_file, int msgid_nodo_rcv_end, int shmid_all_ended, int my_position, NodoHypercube my_nodo)
{
    struct end_msg msg_2_snd;
    struct end_msg msg_exec_end;
    struct msg_nodo msg_exec_name;

    int exec_end, exec_init;

    int pid;
    // all_ended = (int *)shmat(shmid_all_ended, (char *)0, 0);

    msg_exec_name.pid = -1;
    msg_exec_end.position = -1;

    int snd_end_neighbor = 50;
    for (int i = 0; i < 4; i++)
    {
        if (my_nodo.neighbor[i] < snd_end_neighbor)
            snd_end_neighbor = my_nodo.neighbor[i];
    }

    while (1)
    {

        /* MSG DO ESCALONADOR */
        msgrcv(msgid_nodo_snd_file, &msg_exec_name, sizeof(msg_exec_name) - sizeof(long), my_position + 1, IPC_NOWAIT);

        if (msg_exec_name.pid != -1)
        {
            /* MANDA PARA OS VIZINHOS */
            for (int i = 0; i < 4; i++)
            {
                msg_exec_name.pid = my_nodo.neighbor[i] + 1;
                msgsnd(msgid_nodo_snd_file, &msg_exec_name, sizeof(msg_exec_name) - sizeof(long), 0);
            }

            // separa nome arq do path
            char filename[100];
            strcpy(filename, basename(msg_exec_name.arq_executavel));

            // printf("RECEBE ARQUIVO %s\n",  msg_exec_name.arq_executavel);

            //TODO ACABAR O PROGRAMA
            if ((pid = fork()) < 0)
            {
                printf("Error on fork() -> %d\n", errno);
                continue;
            }

            exec_init = (int)time(NULL);
            if (pid == 0)
            {
                execl(msg_exec_name.arq_executavel, filename, (char *)0);
            }

            // espera no atual esperar de executar
            int state;
            wait(&state);
            exec_end = (int)time(NULL);

            // manda mensagem de volta com
            // printf("END NEIGHBOR %d NO %d\n", snd_end_neighbor, my_position);
            msg_2_snd.position = snd_end_neighbor + 1;
            msg_2_snd.end_info[0] = my_position;
            msg_2_snd.end_info[1] = exec_init;
            msg_2_snd.end_info[2] = exec_end;

            msgsnd(msgid_nodo_rcv_end, &msg_2_snd, sizeof(msg_2_snd) - sizeof(long), 0);
            while (1)
            {
                // printf("ESPERANDO MSG FILHOS NO %d\n", my_position);
                /* MSG DOS FILHOS */
                msgrcv(msgid_nodo_rcv_end, &msg_exec_end, sizeof(msg_exec_end) - sizeof(long), my_position + 1, IPC_NOWAIT);

                if (msg_exec_end.position != -1)
                {
                    msg_exec_end.position = snd_end_neighbor + 1;
                    msgsnd(msgid_nodo_rcv_end, &msg_exec_end, sizeof(msg_exec_end) - sizeof(long), 0);
                    msg_exec_end.position = -1;
                }

                // msgrcv(msgid_nodo_snd_file, &msg_all_ended, sizeof(msg_all_ended) - sizeof(long), my_position + ALL_ENDED_DELTA, IPC_NOWAIT);

                // if (msg_all_ended.id != -1)
                // {
                //     for (int i = 0; i < 4; i++)
                //     {
                //         /* MANDA PARA OS VIZINHOS */
                //         msg_all_ended.id = my_nodo.neighbor[i] + ALL_ENDED_DELTA;
                //         msgsnd(msgid_nodo_snd_file, &msg_all_ended, sizeof(msg_all_ended) - sizeof(long), 0);
                //     }

                //     msg_all_ended.id = snd_end_neighbor + ALL_ENDED_DELTA;
                //     msgsnd(msgid_nodo_rcv_end, &msg_all_ended, sizeof(msg_all_ended) - sizeof(long), 0);
                //     break;
                // }
            }

            msg_exec_name.pid = -1;
        }
    }

    shmdt((char *)0);
}

// void wait_all_ended(int msgid_nodo_snd_file, int msgid_nodo_rcv_end, struct msg_all_ended msg_all_ended)
// {
//     int count_ended = 15;

//     msg_all_ended.id = -1;

//     while (count_ended != 0)
//     {
//         msgrcv(msgid_nodo_rcv_end, &msg_all_ended, sizeof(&msg_all_ended) - sizeof(long), ALL_ENDED_DELTA, IPC_NOWAIT);
//         if (msg_all_ended.id != -1)
//         {
//             printf("MSG RECEIVED FROM %ld \n", msg_all_ended.id);
//             count_ended--;
//             msg_all_ended.id = -1;
//         }
//     }

//     do
//     {
//         msg_all_ended.id = -1;
//         msgrcv(msgid_nodo_snd_file, &msg_all_ended, sizeof(msg_all_ended) - sizeof(long), 0, IPC_NOWAIT);
//     } while (msg_all_ended.id != -1);

//     msg_all_ended.id = getpid();
//     msgsnd(msgid_nodo_rcv_end, &msg_all_ended, sizeof(msg_all_ended) - sizeof(long), 0);
// }

void nodo_0_loop_hypercube(int msgid_nodo_snd_file, int msgid_nodo_rcv_end, int shmid_all_ended, NodoHypercube my_nodo)
{
    struct end_msg msg_2_snd;
    struct end_msg msg_exec_end;

    struct msg_nodo msg_exec_name;
    int exec_end, exec_init;

    int pid;

    msg_exec_name.pid = -1;
    msg_exec_end.position = -1;

    while (1)
    {

        /* MSG DO ESCALONADOR */
        msgrcv(msgid_nodo_snd_file, &msg_exec_name, sizeof(msg_exec_name) - sizeof(long), getpid(), IPC_NOWAIT);

        if (msg_exec_name.pid != -1)
        {
            /* MANDA PARA OS VIZINHOS */
            for (int i = 0; i < 4; i++)
            {
                msg_exec_name.pid = my_nodo.neighbor[i] + 1;
                msgsnd(msgid_nodo_snd_file, &msg_exec_name, sizeof(msg_exec_name) - sizeof(long), 0);
            }

            // separa nome arq do path
            char filename[100];
            strcpy(filename, basename(msg_exec_name.arq_executavel));

            // printf("RECEBE ARQUIVO %s\n",  msg_exec_name.arq_executavel);

            //TODO ACABAR O PROGRAMA
            if ((pid = fork()) < 0)
            {
                printf("Error on fork() -> %d\n", errno);
                continue;
            }

            exec_init = (int)time(NULL);
            if (pid == 0)
            {
                if (execl(msg_exec_name.arq_executavel, filename, (char *)0) < 0)
                {
                    printf("ERR: execl failed: %d\n", errno);
                }
            }

            // espera no atual esperar de executar
            int state;
            wait(&state);

            exec_end = (int)time(NULL);

            // manda mensagem de volta com
            msg_2_snd.position = getpid();
            msg_2_snd.end_info[0] = 0;
            msg_2_snd.end_info[1] = exec_init;
            msg_2_snd.end_info[2] = exec_end;

            msgsnd(msgid_nodo_rcv_end, &msg_2_snd, sizeof(msg_2_snd) - sizeof(long), 0);
            printf("NÓ 0 TERMINOU!\n");
            while (1)
            {
                /* MSG DOS FILHOS */
                msgrcv(msgid_nodo_rcv_end, &msg_exec_end, sizeof(msg_exec_end) - sizeof(long), 1, IPC_NOWAIT);
                if (msg_exec_end.position != -1)
                {
                    msg_exec_end.position = getpid();
                    msgsnd(msgid_nodo_rcv_end, &msg_exec_end, sizeof(msg_exec_end) - sizeof(long), 0);
                    msg_exec_end.position = -1;
                }
                /* 
                msgrcv(msgid_nodo_snd_file, &msg_all_ended, sizeof(&msg_all_ended) - sizeof(long), ALL_ENDED_DELTA, IPC_NOWAIT);
                if (msg_all_ended.id != -1)
                {
                    printf("MSG RECEBIDA ALL_ENDED!!\n");
                    for (int i = 0; i < 4; i++)
                    {
                        /* MANDA PARA OS VIZINHOS 
                        msg_all_ended.id = my_nodo.neighbor[i] + ALL_ENDED_DELTA;
                        printf("VIZINHOS NÓ 0: %ld\n", msg_all_ended.id);
                        msgsnd(msgid_nodo_snd_file, &msg_all_ended, sizeof(msg_all_ended) - sizeof(long), 0);
                    }
                    wait_all_ended(msgid_nodo_snd_file, msgid_nodo_rcv_end, msg_all_ended);
                    break; 
                
                }
                */
            }
            printf("NÓ 0 SAIU ALL_ENDED!\n");

            msg_exec_name.pid = -1;
        }
    }

    // shmdt((char *)0);
}
#endif