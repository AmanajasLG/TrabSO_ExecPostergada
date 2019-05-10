#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/msg.h>

#include "executa_postergado.h"

int cfile_exists(const char * filename){
    FILE *file;
    if (file = fopen(filename, "r")){
        fclose(file);
        return 1;
    }
    return 0;
}
int main(int argc, char const *argv[]){
    char *end;
    char arq_executavel[100];
    struct msg msg_2_send;

    if(argc != 3){
        printf("Number of args invalid\n");
    }
    
    strtol(argv[1], &end, 10);
    if (*end != '\0') {
        printf("First arg not a int\n");
    }else{
        msg_2_send.post->sec= argv[1];
    }

    if (!cfile_exists(argv[2])){
        printf("File dont exists\n");
    }else{
        msg_2_send.post->arq_executavel = argv[2];
    }
    if(msgid = msgget(KEY, 0x1FC) < 0){
        msgsnd(msgid, &msg_2_send, sizeof(msg_2_send)-sizeof(long), 0);
    }

    return 0;
}