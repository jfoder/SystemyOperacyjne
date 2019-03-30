#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>

sig_atomic_t stopped = 0;
pid_t child_process;
int has_child_process = 0;

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
    while(1) {
        if(stopped == 0 && has_child_process == 0) {
            has_child_process = 1;
            child_process = fork();
            if (child_process == 0) {
                execlp("./date.sh", "./date.sh", NULL);
            }
        }
        else if(stopped == 1){
            kill(child_process, SIGKILL);
            has_child_process = 0;
        }
    }
    return 0;
}


