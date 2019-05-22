#include "escalonador.h"

int main(int argc, char const *argv[])
{
    int pid, msgid_escale, current_time, nodo0_pid, alarm_countdown, msgid_nodo_rcv_end;
    struct msg msg_from_exec_post;
    struct end_msg msg_from_nodo0;

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

    if ((msgid_escale = msgget(KEY_ESCALE, IPC_CREAT | 0x1FF)) < 0)
    {
        printf("MSG escale List didnt able to be created");
        exit(1);
    }

    if ((msgid_nodo_rcv_end = msgget(KEY_NODO_FILE, IPC_CREAT | 0x1FF)) < 0)
    {
        printf("MSG nodo_rcv List didnt able to be created");
        exit(1);
    }

    if ((msgid_nodo_snd_file = msgget(KEY_NODO_END, IPC_CREAT | 0x1FF)) < 0)
    {
        printf("MSG nodo_snd List didnt able to be created");
        exit(1);
    }

    ready_queue = start_queue();
    run_queue = start_queue();

    if (ready_queue == NULL || run_queue == NULL)
    {
        printf("Fila não criada\n");
        exit(1);
    }

    printf("msgid_escale = %d\n", msgid_escale);

    sleep(10);

    NodoHypercube hypercube[16];
    NodoTorus torus[4][4];
    NodoList list[3];
    TreeNodo fattree[15];

    int my_position_x, my_position_y, count_end_origin;

    switch (topology)
    {

    case HYPERCUBE:
        create_hypercube(hypercube);
        print_hypercube(hypercube);
        count_end_origin = 16;
        break;
    case TORUS:
        create_torus(torus);
        print_torus(torus);
        // for (int i = 0; i < 4; i++)
        // {
        //     for (int j = 0; j < 4; j++)
        //     {
        //         pid = fork();
        //         if (pid != 0)
        //         {
        //             torus[i][j].pid = pid;
        //         }
        //         if (pid == 0)
        //             break;
        //         else if (pid < 0)
        //         {
        //             perror("fork");
        //             exit(1);
        //         }
        //     }
        //     if (pid == 0)
        //         break;
        // }
        exit(0);
        count_end_origin = 16;
        break;
    case FATTREE:
        create_tree(fattree);

        for (my_position_x = 0; my_position_x < 15; my_position_x++)
        {
            pid = fork();
            if (pid == 0)
                break;
            else if (pid < 0)
            {
                perror("fork");
                exit(1);
            }

            fattree[my_position_x].pid = pid;
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

        for (my_position_x = 0; my_position_x < 3; my_position_x++)
        {
            pid = fork();
            if (pid == 0)
                break;
            else if (pid < 0)
            {
                perror("fork");
                exit(1);
            }
            if(my_position_x == 0){
                pid_nodo0 = pid;
            }
        }
        count_end_origin = 3;
    }

    // escalonador
    if (pid != 0)
    {
        signal(SIGALRM, manda_exec_prog);

        time_t exec_end;
        int count_end = count_end_origin;
        // para primeiro arquivo
        msgrcv(msgid_escale, &msg_from_exec_post, sizeof(msg_from_exec_post) - sizeof(long), 0, 0);
        insert_queue(ready_queue, msg_from_exec_post);
        print_queue(ready_queue);
        strcpy(msg_2_nodo0.arq_executavel, msg_from_exec_post.arq_executavel);
        current_time = alarm((int)msg_from_exec_post.sec);
        printf("arquivo executavel copiado\n");

        msg_from_exec_post.sec = -1;
        msg_from_nodo0.position = -1;
        printf("msgid_nodo_snd_file = %d\n", msgid_nodo_snd_file);
        while (1)
        {

            //printf("msg_from_exec_post antes: %ld\n", msg_from_exec_post.sec);

            msgrcv(msgid_escale, &msg_from_exec_post, sizeof(msg_from_exec_post) - sizeof(long), 0, IPC_NOWAIT);
            msgrcv(msgid_nodo_rcv_end, &msg_from_nodo0, sizeof(msg_from_nodo0) - sizeof(long), pid_nodo0, IPC_NOWAIT);

            //printf("msg_from_exec_post depois: %ld\n", msg_from_exec_post.sec);
            // -1 significa que n chegou mensagem
            if (msg_from_exec_post.sec != -1)
            {
                printf("entrou nessa porra 1/2\n");
                alarm_countdown = alarm(0);
                printf("entrou nessa porra\n");
                att_time(alarm_countdown, ready_queue);
                print_queue(ready_queue);
                if (msg_from_exec_post.sec < alarm_countdown)
                {
                    insert_queue_first_pos(ready_queue, msg_from_exec_post);
                    print_queue(ready_queue);
                    alarm(msg_from_exec_post.sec);
                }
                else
                {
                    insert_queue(ready_queue, msg_from_exec_post);
                    print_queue(ready_queue);
                    alarm(alarm_countdown);
                }
                msg_from_exec_post.sec = -1;
            }

            if (msg_from_nodo0.position != -1)
            {
                count_end--;

                printf("No %d terminou:\n", msg_from_nodo0.end_info[0]);
                printf("Hora de inicio: %d\n", msg_from_nodo0.end_info[1]);
                printf("Hora de termino: %d\n", msg_from_nodo0.end_info[2]);

                if(count_end == 0){
                    exec_end = time(NULL);
                    is_executing = false;


                    strcpy(msg_2_nodo0.arq_executavel, " ");
                    msgsnd(msgid_nodo_snd_file, &msg_2_nodo0, sizeof(msg_2_nodo0) - sizeof(long), 0);

                    if (!is_empty(run_queue))
                    {
                        strcpy(msg_2_nodo0.arq_executavel, run_queue->init->arq_executavel);
                        manda_exec_prog();
                    }
                    count_end = count_end_origin;    
                }
                // printf("Tempo total de exec: %d s", exec_init - exec_end);
                
                msg_from_nodo0.position = -1;
            }
        }
    }

    if (pid == 0)
    {

        struct end_msg msg_2_snd, msg_2_rcv_end;
        struct msg_nodo msg_2_rcv;
        char executing[100] = " ";
        int position_2_send = 20;
        int exec_end_nodo, exec_init_nodo;

        for (int i = 0; i < 4; i++)
        {
            if (list[my_position_x].neighbor[i] == -1)
                break;

            if (list[my_position_x].neighbor[i] < position_2_send)
                position_2_send = list[my_position_x].neighbor[i];
        }
        printf("my_position_x: %d\n", my_position_x);
        if(my_position_x == 0){
            while (1)
            {    
                if(my_position_x == 0){
                    
                    printf("mypid: %d\n", getpid());
                    msgrcv(msgid_nodo_snd_file, &msg_2_rcv, sizeof(msg_2_rcv) - sizeof(long), getpid(), 0);    
                    printf("mgs_rcv: %s\n", msg_2_rcv.arq_executavel);
                }
                msgrcv(msgid_nodo_snd_file, &msg_2_rcv, sizeof(msg_2_rcv) - sizeof(long), 0, IPC_NOWAIT);
                

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

                // testa se prog mandado ja ta executando
                if (strcmp(executing, filename))
                    continue;

                strcpy(executing, filename);

                printf("RECEBE ARQUIVO %s\n", executing);

                if ((pid = fork()) < 0)
                {
                    printf("Error on fork() -> %d", errno);
                    continue;
                }
                
                exec_init_nodo = (int)time(NULL);
                if (pid == 0)
                {
                    execl(msg_2_rcv.arq_executavel, filename, (char *)0);
                }

                // espera no atual esperar de executar
                int state;
                wait(&state);
                
                exec_end_nodo = (int)time(NULL);
                printf("Nodo %d terminou de executar %s\n", my_position_x, msg_2_rcv.arq_executavel);

                // manda mensagem de volta com
                msg_2_snd.position = position_2_send;
                msg_2_snd.end_info[0] = my_position_x;
                msg_2_snd.end_info[1] = exec_init_nodo;
                msg_2_snd.end_info[2] = exec_end_nodo;

                msgsnd(msgid_nodo_rcv_end, &msg_2_snd, sizeof(msg_2_snd) - sizeof(long), 0);
            }
        }else{
            msg_2_rcv_end.position = -1;
            msg_2_rcv.pid = -1;
            while (1)
            {
                msgrcv(msgid_nodo_snd_file, &msg_2_rcv, sizeof(msg_2_rcv) - sizeof(long), 0, IPC_NOWAIT);
                
                msgrcv(msgid_nodo_rcv_end, &msg_2_rcv_end, sizeof(msg_2_rcv_end) - sizeof(long), 0, IPC_NOWAIT);

                if(msg_2_rcv_end.position != -1){
                    msg_2_rcv_end.position = position_2_send;
                    msgsnd(msgid_nodo_rcv_end, &msg_2_rcv_end, sizeof(msg_2_rcv_end) - sizeof(long), 0);
                    
                    msg_2_rcv_end.position = -1;                   
                }

                if(msg_2_rcv.pid != -1){
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
                    //TODO : ARQUIVO MSM NOME QUEBRA
                    // testa se prog mandado ja ta executando
                    
                    for(int i = 0; i < 4; i++){
                        msgsnd(msgid_nodo_snd_file, &msg_2_nodo0, sizeof(msg_2_nodo0) - sizeof(long), 0);
                    }

                    strcpy(executing, filename);

                    printf("RECEBE ARQUIVO %s\n", executing);

                    if ((pid = fork()) < 0)
                    {
                        printf("Error on fork() -> %d", errno);
                        continue;
                    }

                    if (pid == 0)
                    {
                        execl(msg_2_rcv.arq_executavel, filename, (char *)0);
                    }

                    // espera no atual esperar de executar
                    int state;
                    wait(&state);
                    printf("Nodo %d terminou de executar %s\n", my_position_x, msg_2_rcv.arq_executavel);

                    // manda mensagem de volta com
                    msg_2_snd.position = position_2_send + 1;
                    // msg_2_snd.position_end = my_position_x;
                    msg_2_snd.end_info[0] = my_position_x;
                    msgsnd(msgid_nodo_rcv_end, &msg_2_snd, sizeof(msg_2_snd) - sizeof(long), 0);
                    msg_2_rcv.pid = -1;
                }
                
            }
        }
        
    }

    struct msqid_ds *buf;
    msgctl(msgid_escale, IPC_RMID, buf);
    msgctl(msgid_nodo_snd_file, IPC_RMID, buf);
    msgctl(msgid_nodo_rcv_end, IPC_RMID, buf);
    free_queue(ready_queue);
    free_queue(run_queue);

    return 0;
}
