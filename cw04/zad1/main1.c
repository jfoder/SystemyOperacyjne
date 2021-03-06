#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>

sig_atomic_t stopped = 0;

void catchSIGTSTP(int signum){
    printf("\rPress CTRL+Z to continue or CTRL+C to terminate process\n");
    if(stopped == 0) {
        stopped = 1;
        signal(SIGTSTP, catchSIGTSTP);
        return;
    }
    else {
        stopped = 0;
        signal(SIGTSTP, catchSIGTSTP);
        return;
    }
}

void catchSIGINT(int signum){
    stopped = 0;
    printf("\rProcess has been terminated successfully\n");
    exit(0);
}


int main(int argc, char* argv[]) {
    struct sigaction action;
    action.sa_handler = catchSIGTSTP;
    sigaction(SIGTSTP, &action, NULL);
    signal(SIGINT, catchSIGINT);

    time_t t;
    struct tm* tm;
    while(1) {
        if(stopped == 1) continue;
        time(&t);
        tm = localtime(&t);
        printf(asctime(tm));
        sleep(1);
    }
    return 0;
}


