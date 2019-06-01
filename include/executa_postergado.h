/**
 * @authors: 
 * @name Luíza Amanajás
 * @matricula 160056659
 */

#ifndef EXECUTA_POSTERGADO_H_
#define EXECUTA_POSTERGADO_H_

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/msg.h>
#include <unistd.h>
#include <sys/shm.h>

#define KEY_QUEUE 0x03718
#define KEY_JOB 0x16005

struct msg
{
    long sec;
    char arq_executavel[100];
};

#endif