#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/msg.h>
#include <sys/errno.h>

#include "escalonador.h"

int main(int argc, char const *argv[])
{
    int pid, msgid_exec_post, msgid_escale;
    struct msg msg_2_rcv;
    char arg_error_msg[] = "Como argumento insira qual topologia deseja usar no escalonador:\n0: hypercube\n1: torus\n2: fat tree\nSeu comando deve ser: escalonador <topologia> &\n";

    if (argc < 2 || (argc > 4 && strcmp(argv[2], "&")))
    {
        printf("Numero de argumentos invalidos.\n");
        printf("%s", arg_error_msg);
        return EXIT_FAILURE;
    }

    int topology = atoi(argv[1]);

    if (topology < 0 && topology > 2)
    {
        printf("Argumento passado nao eh valido.\n");
        printf("%s", arg_error_msg);
        return EXIT_FAILURE;
    }

    if ((msgid_exec_post = msgget(KEY_EXEC_POST, IPC_CREAT | 0x1FF)) < 0)
    {
        printf("MSG List didnt able to be created");
        exit(1);
    }

    if ((msgid_escale = msgget(KEY_ESCALE, IPC_CREAT | 0x1FF)) < 0)
    {
        printf("MSG List didnt able to be created");
        exit(1);
    }

    // printf("msgid_exec_post = %d\n", msgid_exec_post);
    // printf("msgid_escale = %d\n", msgid_escale);

    //sleep(10);

    Nodo hypercube[16];
    Nodo torus[4][4];
    TreeNodo fattree[15];

    switch (topology)
    {

    case HYPERCUBE:
        create_hypercube(hypercube);

        break;
    case TORUS:
        create_torus(torus);

        break;
    case FATTREE:
        create_tree(fattree);

        for (int i = 0; i < 15; i++)
        {
            pid = fork();
            if (pid == 0)
            {
                break;
            }
            else if (pid < 0)
            {
                perror("fork");
                exit(1);
            }

            fattree[i].pid = pid;
        }

        break;
    }

    if (pid == 0)
    {
    }

    if (pid != 0)
    {
        while (1)
        {
            printf("passou\n");
            msgrcv(msgid_exec_post, &msg_2_rcv, sizeof(msg_2_rcv) - sizeof(long), 0, 0);
            printf("chegou alguma msg %d\n", errno);
            printf("%ld %s\n", msg_2_rcv.sec, msg_2_rcv.arq_executavel);
            // print_topology(FATTREE, fattree);
        }
    }

    struct msqid_ds *buf;
    msgctl(msgid_exec_post, IPC_RMID, buf);
    msgctl(msgid_escale, IPC_RMID, buf);

    return 0;
}
