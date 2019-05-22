#ifndef TREE_H_

#define TREE_H_

#include "includes.h"

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

/* 
    msg_2_snd -> MSG ENVIADA QUANDO TERMINA DE EXEC
    msg_2_rcv_end -> MSG RECEBIDA QUANDO PROGRAMAS FILHOS ACABAM DE EXEC
    msg_2_rcv -> MSG RECEBIDA COM O NOME DO PROG A SER EXEC

    OBS: ID NA FILA É A POS NO VEC + 1 PQ 0 NÃO PODE SER ID
*/

void nodo_loop_tree(int msgid_nodo_rcv_end, int shmid_all_ended, int my_position, TreeNodo my_nodo)
{
    struct end_msg msg_2_snd;
    struct end_msg msg_2_rcv_end;
    struct msg_nodo msg_2_rcv;
    char executing[100] = " ";
    int exec_end, exec_init;

    int pid;
    all_ended = shmat(shmid_all_ended, (char *)0, 0);

    msg_2_rcv.pid = -1;
    msg_2_rcv_end.position = -1;
    while (1)
    {

        /* MSG DO ESCALONADOR */
        msgrcv(msgid_nodo_snd_file, &msg_2_rcv, sizeof(msg_2_rcv) - sizeof(long), my_nodo.parent + 1, IPC_NOWAIT);

        /* MSG DOS FILHOS */
        msgrcv(msgid_nodo_rcv_end, &msg_2_rcv_end, sizeof(msg_2_rcv_end) - sizeof(long), my_position + 1, IPC_NOWAIT);

        if (msg_2_rcv.pid != -1)
        {
            /* MANDA PARA O FILHO DA ESQUERDA SE HOUVER */
            if (my_nodo.left != -1)
            {
                msg_2_rcv.pid = my_nodo.left + 1;
                msgsnd(msgid_nodo_snd_file, &msg_2_rcv, sizeof(msg_2_rcv) - sizeof(long), 0);
            }

            /* MANDA PARA O FILHO DA DIREITA SE HOUVER */
            if (my_nodo.left != -1)
            {
                msg_2_rcv.pid = my_nodo.right + 1;
                msgsnd(msgid_nodo_snd_file, &msg_2_rcv, sizeof(msg_2_rcv) - sizeof(long), 0);
            }

            // separa nome arq do path
            char *str;
            strcpy(str, msg_2_rcv.arq_executavel);
            int init_size = strlen(str);
            char delim[] = "/";
            char *filename;

            char *ptr = strtok(str, delim);

            while (ptr != NULL)
            {
                filename = ptr;
                ptr = strtok(NULL, delim);
            }

            strcpy(executing, filename);

            printf("RECEBE ARQUIVO %s\n", executing);

            if ((pid = fork()) < 0)
            {
                printf("Error on fork() -> %d", errno);
                continue;
            }

            exec_init = (int)time(NULL);
            if (pid == 0)
            {
                execl(msg_2_rcv.arq_executavel, filename, (char *)0);
            }

            // espera no atual esperar de executar
            int state;
            wait(&state);

            exec_end = (int)time(NULL);
            printf("Nodo %d terminou de executar %s\n", 0, msg_2_rcv.arq_executavel);

            // manda mensagem de volta com
            msg_2_snd.position = my_nodo.parent + 1;
            msg_2_snd.end_info[0] = my_position;
            msg_2_snd.end_info[1] = exec_init;
            msg_2_snd.end_info[2] = exec_end;

            msgsnd(msgid_nodo_rcv_end, &msg_2_snd, sizeof(msg_2_snd) - sizeof(long), 0);

            while (!(bool *)all_ended)
                ;

            msg_2_rcv.pid = -1;
        }

        if (msg_2_rcv_end.position != -1)
        {
            msg_2_rcv_end.position = my_nodo.parent + 1;
            msgsnd(msgid_nodo_rcv_end, &msg_2_snd, sizeof(msg_2_snd) - sizeof(long), 0);
            msg_2_rcv_end.position = -1;
        }
    }
}

void nodo_0_loop_tree(int msgid_nodo_rcv_end, int shmid_all_ended, TreeNodo my_nodo)
{
    struct end_msg msg_2_snd;
    struct end_msg msg_2_rcv_end;
    struct msg_nodo msg_2_rcv;
    char executing[100] = " ";
    int exec_end, exec_init;
    int pid;
    all_ended = shmat(shmid_all_ended, (char *)0, 0);

    msg_2_rcv.pid = -1;
    msg_2_rcv_end.position = -1;
    while (1)
    {

        /* MSG DO ESCALONADOR */
        msgrcv(msgid_nodo_snd_file, &msg_2_rcv, sizeof(msg_2_rcv) - sizeof(long), getpid(), IPC_NOWAIT);

        /* MSG DOS FILHOS */
        msgrcv(msgid_nodo_rcv_end, &msg_2_rcv_end, sizeof(msg_2_rcv_end) - sizeof(long), 1, IPC_NOWAIT);

        if (msg_2_rcv.pid != -1)
        {
            /* MANDA PARA O FILHO DA ESQUERDA */
            msg_2_rcv.pid = my_nodo.left + 1;
            msgsnd(msgid_nodo_snd_file, &msg_2_rcv, sizeof(msg_2_rcv) - sizeof(long), 0);

            /* MANDA PARA O FILHO DA DIREITA */
            msg_2_rcv.pid = my_nodo.right + 1;
            msgsnd(msgid_nodo_snd_file, &msg_2_rcv, sizeof(msg_2_rcv) - sizeof(long), 0);

            // separa nome arq do path
            char *str;
            strcpy(str, msg_2_rcv.arq_executavel);
            int init_size = strlen(str);
            char delim[] = "/";
            char *filename;

            char *ptr = strtok(str, delim);

            while (ptr != NULL)
            {
                filename = ptr;
                ptr = strtok(NULL, delim);
            }

            strcpy(executing, filename);

            printf("RECEBE ARQUIVO %s\n", executing);

            if ((pid = fork()) < 0)
            {
                printf("Error on fork() -> %d", errno);
                continue;
            }

            exec_init = (int)time(NULL);
            if (pid == 0)
            {
                execl(msg_2_rcv.arq_executavel, filename, (char *)0);
            }

            // espera no atual esperar de executar
            int state;
            wait(&state);

            exec_end = (int)time(NULL);
            printf("Nodo %d terminou de executar %s\n", 0, msg_2_rcv.arq_executavel);

            // manda mensagem de volta
            msg_2_snd.position = getpid();
            msg_2_snd.end_info[0] = 0;
            msg_2_snd.end_info[1] = exec_init;
            msg_2_snd.end_info[2] = exec_end;

            msgsnd(msgid_nodo_rcv_end, &msg_2_snd, sizeof(msg_2_snd) - sizeof(long), 0);

            while (!(bool *)all_ended)
                ;

            msg_2_rcv.pid = -1;
        }

        if (msg_2_rcv_end.position != -1)
        {
            msg_2_rcv_end.position = getpid();
            msgsnd(msgid_nodo_rcv_end, &msg_2_snd, sizeof(msg_2_snd) - sizeof(long), 0);
            msg_2_rcv_end.position = -1;
        }
    }
}

#endif