/**
 * @authors: 
 * @name Luíza Amanajás
 * @matricula 160056659
 * @name Yuri Castro do Amaral
 * @matricula 140033718
 */

#ifndef TREE_H_

#define TREE_H_

#include "includes.h"

void create_tree(TreeNodo tree[15])
{
    tree[0].msg_rcv_number = 14;
    tree[1].msg_rcv_number = 6;
    tree[2].msg_rcv_number = 6;
    tree[3].msg_rcv_number = 2;
    tree[4].msg_rcv_number = 2;
    tree[5].msg_rcv_number = 2;
    tree[6].msg_rcv_number = 2;
    tree[7].msg_rcv_number = 0;
    tree[8].msg_rcv_number = 0;
    tree[9].msg_rcv_number = 0;
    tree[10].msg_rcv_number = 0;
    tree[11].msg_rcv_number = 0;
    tree[12].msg_rcv_number = 0;
    tree[13].msg_rcv_number = 0;
    tree[14].msg_rcv_number = 0;
    tree[15].msg_rcv_number = 0;

    for (int i = 0; i < 15; i++)
    {
        int right_index = 2 * i + 2;
        int left_index = 2 * i + 1;

        if (i <= 2)
        {
            if (i == 0)
            {
                tree[i].parent = -1;
            }

            tree[right_index].parent = i;
            tree[i].right = right_index;

            tree[left_index].parent = i;
            tree[i].left = left_index;
        }
        else
        {
            if (i < 7)
            {
                tree[i].right = right_index;
                tree[i].left = left_index;
            }
            else
            {
                tree[i].right = -1;
                tree[i].left = -1;
            }

            tree[right_index].parent = i;
            tree[left_index].parent = i;
        }
    }
}

void print_tree(TreeNodo tree[15])
{
    for (int i = 0; i < 15; i++)
        printf("nodo [%d] pid %d pai: %d dir %d esq %d\n", i, tree[i].pid, tree[i].parent, tree[i].right, tree[i].left);
}

/* 
    msg_2_snd -> MSG ENVIADA QUANDO TERMINA DE EXEC
    msg_exec_end -> MSG RECEBIDA QUANDO PROGRAMAS FILHOS ACABAM DE EXEC
    msg_exec_name -> MSG RECEBIDA COM O NOME DO PROG A SER EXEC

    OBS: ID NA FILA É A POS NO VEC + 1 PQ 0 NÃO PODE SER ID
*/

void nodo_loop_tree(int my_position, TreeNodo my_nodo)
{
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
            /* MANDA PARA O FILHO DA ESQUERDA SE HOUVER */
            if (my_nodo.left != -1)
            {
                msg_exec_name.id = my_nodo.left + 1;
                msgsnd(msgid_nodo_snd_file, &msg_exec_name, sizeof(msg_exec_name) - sizeof(long), 0);
            }

            /* MANDA PARA O FILHO DA DIREITA SE HOUVER */
            if (my_nodo.left != -1)
            {
                msg_exec_name.id = my_nodo.right + 1;
                msgsnd(msgid_nodo_snd_file, &msg_exec_name, sizeof(msg_exec_name) - sizeof(long), 0);
            }
            // separa nome arq do path
            char filename[100];
            strcpy(filename, basename(msg_exec_name.arq_executavel));

            // printf("RECEBE ARQUIVO %s\n",  msg_exec_name.arq_executavel);

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
            msg_2_snd.position = my_nodo.parent + 1;
            msg_2_snd.end_info[0] = my_position;
            msg_2_snd.end_info[1] = exec_init;
            msg_2_snd.end_info[2] = exec_end;

            msgsnd(msgid_nodo_rcv_end, &msg_2_snd, sizeof(msg_2_snd) - sizeof(long), 0);

            while (my_position < 7)
            {
                /* MSG DOS FILHOS */
                msgrcv(msgid_nodo_rcv_end, &msg_exec_end, sizeof(msg_exec_end) - sizeof(long), my_position + 1, IPC_NOWAIT);
                if (msg_exec_end.position != -1)
                {
                    msg_exec_end.position = my_nodo.parent + 1;
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

void nodo_0_loop_tree(TreeNodo my_nodo)
{
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
        msgrcv(msgid_nodo_snd_file, &msg_exec_name, sizeof(msg_exec_name) - sizeof(long), getpid(), IPC_NOWAIT);

        if (msg_exec_name.id != -1)
        {
            /* MANDA PARA O FILHO DA ESQUERDA */
            msg_exec_name.id = my_nodo.left + 1;
            msgsnd(msgid_nodo_snd_file, &msg_exec_name, sizeof(msg_exec_name) - sizeof(long), 0);

            /* MANDA PARA O FILHO DA DIREITA */
            msg_exec_name.id = my_nodo.right + 1;
            msgsnd(msgid_nodo_snd_file, &msg_exec_name, sizeof(msg_exec_name) - sizeof(long), 0);

            // separa nome arq do path

            char filename[100];
            strcpy(filename, basename(msg_exec_name.arq_executavel));

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

            // manda mensagem de volta
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