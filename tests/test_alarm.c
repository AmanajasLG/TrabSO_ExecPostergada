#include <stdio.h>
#include <unistd.h>


int main (){

    int x;

    alarm(50);
    sleep(5);


    x = alarm(0);
    printf("tempo %d", x);

    return 0;
}