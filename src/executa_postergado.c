#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/msg.h>

#include "executa_postergado.h"

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
    int msgid_escale;

    char *end;
    // char arq_executavel[100];
    struct msg msg_2_send;

    if (argc != 3)
    {
        printf("[executa-postergado] Invalid number of args\n");
        exit(1);
    }
    strtol(argv[1], &end, 10);
    if (*end != '\0')
    {
        printf("[executa-postergado] First arg not a int\n");
        exit(1);
    }
    else
    {
        msg_2_send.sec = atoi(argv[1]);
    }

    if (!cfile_exists(argv[2]))
    {
        printf("[executa-postergado] File dont exists\n");
        exit(1);
    }
    else
    {
        strcpy(msg_2_send.arq_executavel, argv[2]);
    }

    if ((msgid_escale = msgget(KEY_ESCALE, 0x1FF)) < 0)
    {
        printf("[executa-postergado] MSG List didnt able to be created");
        exit(1);
    }
    else
    {
        printf("[executa-postergado] msgid_escale = %d\n", msgid_escale);
        printf("[executa-postergado] msg_2_send: sec[%ld] exec[%s] to list[%d]\n", msg_2_send.sec, msg_2_send.arq_executavel, msgid_escale);
        msgsnd(msgid_escale, &msg_2_send, sizeof(msg_2_send) - sizeof(long), 0);
    }

    return 0;
}