// #ifndef EXECUTA_POSTERGADO_H_
// #define EXECUTA_POSTERGADO_H_

#define KEY 0x3718

struct msg{
    long mtype;
    struct postergado* post;
};

struct postergado {
    char arq_executavel[100];
    int sec;
};