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


int main(int argc, char const *argv[]){
    int pid, msgid;
    struct msg msg_2_rcv;
    char arg_error_msg[] = "Como argumento insira qual topologia deseja usar no escalonador:\n0: hypercube\n1: torus\n2: fat tree\nSeu comando deve ser: escalonador <topologia> &\n";

    if (argc < 2 || (argc > 4 && strcmp(argv[2],"&"))) {
        printf("Numero de argumentos invalidos.\n");
        printf("%s", arg_error_msg);
        return EXIT_FAILURE;
    }

    int topology = atoi(argv[1]);

    if(topology < 0 && topology > 2){
        printf("Argumento passado nao eh valido.\n");
        printf("%s", arg_error_msg);
        return EXIT_FAILURE;
    }

    Nodo hypercube[16];
    Nodo torus[4][4];
    TreeNodo fattree[15];
 
    if ((msgid = msgget(KEY, IPC_CREAT|0x1FF)) < 0){
        printf("MSG List didnt able to be created");
        exit(1);
    }
    
    printf("passou");
    msgrcv(msgid, &msg_2_rcv, sizeof(msg_2_rcv)-sizeof(long), 0, 0);
    printf("chegou alguma msg %d\n", errno);

    printf("%ld %s\n",msg_2_rcv.sec, msg_2_rcv.arq_executavel ); 

    for(int i = 0; i < 15; i++){
        int right_index = 2 * i +2;
        int left_index = 2 * i +1;

        if (i <= 2) {
            if (i == 0) {
                fattree[i].parent = -1;
            }
            
            fattree[right_index].parent = i;
            fattree[i].right = right_index;
            fattree[i].right_extra = right_index;

            fattree[left_index].parent = i;
            fattree[i].left = left_index;
            fattree[i].left_extra = left_index;
        }else{
            if(i < 7){
                fattree[i].right = right_index;
                fattree[i].left = left_index;
            } else{
                fattree[i].right = -1;
                fattree[i].left = -1;
            }

            fattree[right_index].parent = i;
            fattree[i].right_extra = -1;
            fattree[left_index].parent = i;
            fattree[i].left_extra = -1;            
        }
        
    }
    
    switch (topology){ 
        
        case HYPERCUBE:
            break;
        case TORUS:
            break;
        case FATTREE:
            for(int i = 0; i < 15; i++){
                pid = fork();
                if (pid == 0) {
                    break;
                }else if (pid < 0){
                    perror("fork");
                    exit(1);
                }
                
                fattree[i].pid = pid;
            }
                
            break;
    }

    if (pid != 0) {
        print_topology(FATTREE,fattree);
    }
    
    
    return 0;
}
