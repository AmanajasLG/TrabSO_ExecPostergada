/**
 * @authors: 
 * @name Luíza Amanajás
 * @matricula 160056659
 * @name Yuri Castro do Amaral
 * @matricula 140033718
 */

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
    int msgid, shmid;

    char *end;
    // char arq_executavel[100];
    struct msg msg_2_send;

    if (argc != 3)
    {
        printf("Numero invalido de argumentos!\n");
        exit(1);
    }
    strtol(argv[1], &end, 10);
    if (*end != '\0')
    {
        printf("Primeiro argumento deve ser um inteiro!\n");
        exit(1);
    }
    else
    {
        msg_2_send.sec = atoi(argv[1]);
    }

    if (access(argv[2], F_OK) == -1)
    {
        printf("O arquivo enviado nao existe! Verifique o nome ou o path enviado!\n");
        exit(1);
    }
    else
    {
        strcpy(msg_2_send.arq_executavel, argv[2]);
    }

    if ((msgid = msgget(KEY_QUEUE, 0x1FF)) < 0)
    {
        printf("A conexao com lista de mensagem nao foi possivel!\n");
        exit(1);
    }

    if ((shmid = shmget(KEY_JOB, sizeof(int), 0x1FF)) < 0)
    {
        printf("A conexao com memoria compartilhada nao foi possivel!\n");
        exit(1);
    }

    int *job = (int *)shmat(shmid, (void *)0, 0);

    printf("\njob: %d, arq exec: %s, delay: %ld\n", *job, msg_2_send.arq_executavel, msg_2_send.sec);
    msgsnd(msgid, &msg_2_send, sizeof(msg_2_send) - sizeof(long), 0);

    shmdt(job);

    return 0;
}