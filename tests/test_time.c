#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <stdbool.h>

int main(int argc, char const *argv[])
{
    int init, end;
    char time_init[30], time_end[30];

    time_t init_time = time(NULL);

    struct tm *tm_init = localtime(&init_time);
    strftime(time_init, 30, "%d/%m/%Y, %H:%M:%S", tm_init);
    printf("init date 1: %s\n", time_init);

    init = (int)time(NULL);

    sleep(10);
    end = (int)time(NULL);

    time_t end_time = time(NULL);

    sleep(10);

    struct tm *tm_init_2 = localtime(&init_time);
    strftime(time_init, 30, "%d/%m/%Y, %H:%M:%S", tm_init_2);
    printf("init date 2: %s\n", time_init);
    struct tm *tm_end = localtime(&end_time);

    // 
    strftime(time_end, 30, "%d/%m/%Y, %H:%M:%S", tm_end);

    printf("end date: %s\ninit: %ld\nend: %ld\ntotal: %d\n", time_end, init_time, end_time, end - init);

    return 0;
}