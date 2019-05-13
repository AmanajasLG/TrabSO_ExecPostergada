#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/msg.h>
#include <sys/errno.h>
#include <sys/wait.h>
#include <time.h>

#include "escalonador.h"

int main(int argc, char const *argv[])
{
    int pid, msgid_exec_post, msgid_escale, current_time;
    struct msg msg_2_rcv;
    Queue* ready_queue = NULL;
 
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
    
    ready_queue = start_queue();

    if(ready_queue == NULL){
        printf("Fila não criada\n");
        exit(1);
    }



    printf("msgid_exec_post = %d\n", msgid_exec_post);
    printf("msgid_escale = %d\n", msgid_escale);

    sleep(10);

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

    if (pid != 0){
        while (1){      
            
            msgrcv(msgid_exec_post, &msg_2_rcv, sizeof(msg_2_rcv) - sizeof(long), 0, 0);
            insert_queue(ready_queue, msg_2_rcv);  
            print_queue(ready_queue);          
            // current_time = alarm((int) msg_2_rcv.sec);
            // if (current_time == 0){
            //     //executa o prox oque chegou agr
            // }else if(msg_2_rcv < current_time ){
            //     ready_queue->init;
            //     alarm();

            // }
            
            

            // printf("chegou alguma msg %d\n", errno);
            // printf("%ld %s\n", msg_2_rcv.sec, msg_2_rcv.arq_executavel);
            // if(ready_queue->next == NULL){
            //     continue;
            // }
            // print_topology(FATTREE, fattree);
        }
    }


    if (pid == 0)
    {   
        time_t exec_init;
        time_t exec_end;
        struct end_msg msg_2_snd;

        while(1){
            printf("aqui");
            msgrcv(msgid_escale, &msg_2_rcv, sizeof(msg_2_rcv) - sizeof(long), 0, 0);

            exec_init = time(NULL);
            if((pid = fork()) <0){
                printf("Error on fork() -> %d", errno);
                continue;
            }
            
            if(pid == 0){
                char *str;
                strcpy(str, msg_2_rcv.arq_executavel);
                int init_size = strlen(str);
                char delim[] = "/";
                char *filename;

                char *ptr = strtok(str, delim);

                while (ptr != NULL){
                    filename = ptr;
                    ptr = strtok(NULL, delim);
                }
                printf("'%s'\n", filename);
                
                execl(msg_2_rcv.arq_executavel, filename, (char*) 0);
            }   

            int state;
            wait(&state);
            exec_end = time(NULL);
            msg_2_snd.pid = getpid();
            msg_2_snd.exec_time[0] = exec_init;
            msg_2_snd.exec_time[1] = exec_end;
            msgsnd(msgid_escale, &msg_2_snd, sizeof(msg_2_snd) - sizeof(long), 0);
        }
    }
    
    

    struct msqid_ds *buf;
    msgctl(msgid_exec_post, IPC_RMID, buf);
    msgctl(msgid_escale, IPC_RMID, buf);
    free_queue(ready_queue);

    return 0;
}
