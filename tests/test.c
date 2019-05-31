#include <stdio.h>
#include <stdlib.h>

#include <sys/ipc.h>
#include <sys/signal.h>
#include <sys/types.h>
#include <unistd.h>

// void end()
// {
//     exit(0);
// }

int main()
{

    /* signal(SIGALRM, end);
    alarm(10);

    while (1)
        ; */

    sleep(5);

    return 0;
}
