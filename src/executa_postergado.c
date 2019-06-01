#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/msg.h>

#include "../include/executa_postergado.h"

int cfile_exists(const char *filename)
{
    FILE *file;
    if ((file = fopen(filename, "r")))
    {
        fclose(file);
        return 1;
    }
    return 0;
}
int main(int argc, char const *argv[])
{
    int msgid;

    char *end;
    // char arq_executavel[100];
    struct msg msg_2_send;

    if (argc != 3)
    {
        printf("Invalid number of args\n");
        exit(1);
    }
    strtol(argv[1], &end, 10);
    if (*end != '\0')
    {
        printf("First arg must be an int\n");
        exit(1);
    }
    else
    {
        msg_2_send.sec = atoi(argv[1]);
    }

    if (!cfile_exists(argv[2]))
    {
        printf("Invalid file\n");
        exit(1);
    }
    else
    {
        strcpy(msg_2_send.arq_executavel, argv[2]);
    }

    if ((msgid = msgget(KEY, 0x1FF)) < 0)
    {
        printf("MSG List didnt able to be created");
        exit(1);
    }
    else
    {
        printf("\nMSG 2 send %ld %s to %d\n", msg_2_send.sec, msg_2_send.arq_executavel, msgid);
        msgsnd(msgid, &msg_2_send, sizeof(msg_2_send) - sizeof(long), 0);
    }

    return 0;
}