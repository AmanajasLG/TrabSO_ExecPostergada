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

    if ((shmid_all_ended = shmget(KEY_ALL_ENDED, sizeof(int), IPC_CREAT | 0x1ff)) < 0)
    {
        printf("erro na criacao da memoria compartilhada: %d\n", errno);
        exit(1);
    }
    
    signal(SIGTERM, end_program);
    ready_queue = start_queue();
    run_queue = start_queue();
    ended_queue = start_queue();

    if (ready_queue == NULL || run_queue == NULL || ended_queue == NULL)
    {
        printf("Fila não criada\n");
        exit(1);
    }

    sleep(5);

    
    

    int my_position, count_end_origin;

    switch (topology)
    {

    case HYPERCUBE:
        create_hypercube(hypercube);

         for (my_position = 0; my_position < 16; my_position++){
            
            pid = fork();
            if (pid == 0){
                if (my_position == 0)
                {

                    nodo_0_loop_hypercube(msgid_nodo_snd_file, msgid_nodo_rcv_end, shmid_all_ended, hypercube[0]);
                }
                else
                {

                    nodo_loop_hypercube(msgid_nodo_snd_file, msgid_nodo_rcv_end, shmid_all_ended, my_position, hypercube[my_position]);
                }
                break;
            }

            else if (pid < 0)
            {
                perror("fork");
                end_program(msgid_escale, msgid_nodo_rcv_end, shmid_all_ended);
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

        for (my_position = 0; my_position < 16; my_position++){
            
            pid = fork();
            if (pid == 0){
                if (my_position == 0)
                {

                    nodo_0_loop_torus(msgid_nodo_snd_file, msgid_nodo_rcv_end, shmid_all_ended, torus[0]);
                }
                else
                {

                    nodo_loop_torus(msgid_nodo_snd_file, msgid_nodo_rcv_end, shmid_all_ended, my_position, torus[my_position]);
                }
                break;
            }

            else if (pid < 0)
            {
                perror("fork");
                end_program(msgid_escale, msgid_nodo_rcv_end, shmid_all_ended);
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

                    nodo_0_loop_tree(msgid_nodo_snd_file, msgid_nodo_rcv_end, shmid_all_ended, tree[0]);
                }
                else
                {

                    nodo_loop_tree(msgid_nodo_snd_file, msgid_nodo_rcv_end, shmid_all_ended, my_position, tree[my_position]);
                }
                break;
            }

            else if (pid < 0)
            {
                perror("fork");
                end_program(msgid_escale, msgid_nodo_rcv_end, shmid_all_ended);
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
    case LIST:
        list[0].neighbor[0] = 1;
        list[0].neighbor[1] = -1;
        list[0].neighbor[2] = -1;
        list[0].neighbor[3] = -1;

        list[1].neighbor[0] = 0;
        list[1].neighbor[1] = 2;
        list[1].neighbor[2] = -1;
        list[1].neighbor[3] = -1;

        list[2].neighbor[0] = 1;
        list[2].neighbor[1] = -1;
        list[2].neighbor[2] = -1;
        list[2].neighbor[3] = -1;

        for (my_position = 0; my_position < 3; my_position++)
        {
            pid = fork();
            if (pid == 0)
                break;
            else if (pid < 0)
            {
                perror("fork");
                end_program(msgid_escale, msgid_nodo_rcv_end, shmid_all_ended);
                exit(1);
            }
            if (my_position == 0)
            {
                pid_nodo0 = pid;
            }
        }
        count_end_origin = 3;
    }

    // escalonador
    if (pid != 0)
    {
        loop_escalonator(msgid_escale, msgid_nodo_rcv_end, shmid_all_ended, count_end_origin);
    }
    
    return 0;
}
