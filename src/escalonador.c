/**
 * @authors: 
 * @name Luíza Amanajás
 * @matricula 160056659
 */

#include "../include/escalonador.h"

int main(int argc, char const *argv[])
{
    /* VERIFICAÇÃO DE PARÂMETROS ENVIADOS */
    char arg_error_msg[] = "Como argumento insira qual topologia deseja usar no escalonador:\n0: hypercube\n1: torus\n2: fat tree\nSeu comando deve ser: escalonador <topologia> &\n";

    if (argc < 2 || (argc > 4 && strcmp(argv[2], "&")))
    {
        printf("Numero de argumentos invalidos.\n");
        printf("%s", arg_error_msg);
        return EXIT_FAILURE;
    }

    topology = atoi(argv[1]);

    if (topology < 0 && topology > 2)
    {
        printf("Argumento passado nao eh valido.\n");
        printf("%s", arg_error_msg);
        return EXIT_FAILURE;
    }

    /* COMEÇA A GERAR FILAS */
    int pid;

    if ((msgid_escale = msgget(KEY_ESCALE, IPC_CREAT | 0x1FF)) < 0)
    {
        printf("MSG escale List didnt able to be created");
        exit(1);
    }

    if ((msgid_nodo_rcv_end = msgget(KEY_NODO_END, IPC_CREAT | 0x1FF)) < 0)
    {
        printf("MSG nodo_rcv List didnt able to be created");
        exit(1);
    }

    if ((msgid_nodo_snd_file = msgget(KEY_NODO_FILE, IPC_CREAT | 0x1FF)) < 0)
    {
        printf("MSG nodo_snd List didnt able to be created");
        exit(1);
    }

    int my_position, count_end_origin;

    switch (topology)
    {

    case HYPERCUBE:
        create_hypercube(hypercube);

        for (my_position = 0; my_position < 16; my_position++)
        {

            pid = fork();
            if (pid == 0)
            {
                if (my_position == 0)
                {

                    nodo_0_loop_hypercube(hypercube[0]);
                }
                else
                {

                    nodo_loop_hypercube(my_position, hypercube[my_position]);
                }
                break;
            }

            else if (pid < 0)
            {
                perror("fork");
                end_program(msgid_escale, msgid_nodo_rcv_end);
                exit(1);
            }
            if (my_position == 0)
            {
                pid_nodo0 = pid;
            }
            hypercube[my_position].pid = pid;
        }
        count_end_origin = 16;
        break;
    case TORUS:
        create_torus(torus);

        for (my_position = 0; my_position < 16; my_position++)
        {

            pid = fork();
            if (pid == 0)
            {
                if (my_position == 0)
                {

                    nodo_0_loop_torus(torus[0]);
                }
                else
                {

                    nodo_loop_torus(my_position, torus[my_position]);
                }
                break;
            }

            else if (pid < 0)
            {
                perror("fork");
                end_program(msgid_escale, msgid_nodo_rcv_end);
                exit(1);
            }
            if (my_position == 0)
            {
                pid_nodo0 = pid;
            }
            torus[my_position].pid = pid;
        }

        count_end_origin = 16;
        break;
    case TREE:
        create_tree(tree);

        for (my_position = 0; my_position < 15; my_position++)
        {
            pid = fork();
            if (pid == 0)
            {
                if (my_position == 0)
                {

                    nodo_0_loop_tree(tree[0]);
                }
                else
                {

                    nodo_loop_tree(my_position, tree[my_position]);
                }
                break;
            }

            else if (pid < 0)
            {
                perror("fork");
                end_program(msgid_escale, msgid_nodo_rcv_end);
                exit(1);
            }
            if (my_position == 0)
            {
                pid_nodo0 = pid;
            }
            tree[my_position].pid = pid;
        }
        count_end_origin = 15;
        break;
    }

    // escalonador
    if (pid != 0)
    {
        loop_escalonator(count_end_origin);
    }

    return 0;
}