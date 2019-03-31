#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

int signal_count = 0;
int count;
struct sigaction act;

void catchSIGUSR1(int sig, siginfo_t* info, void* context){
    sigaction(SIGUSR1, &act, NULL);
    signal_count++;
}

void catchSIGUSR2(int sig, siginfo_t* info, void* context){
    printf("[SENDER] Signals caught: %d, expected: %d\n", signal_count, count);
    exit(0);
}


int main(int argc, char* argv[]){
    sigset_t blocked;
    sigfillset(&blocked);
    sigdelset(&blocked, SIGUSR1);
    sigdelset(&blocked, SIGUSR2);
    sigprocmask(SIG_SETMASK, &blocked, NULL);

    act.sa_sigaction = catchSIGUSR1;
    sigemptyset(&act.sa_mask);
    sigaction(SIGUSR1, &act, NULL);
    act.sa_sigaction = catchSIGUSR2;
    sigaction(SIGUSR2, &act, NULL);
    act.sa_sigaction = catchSIGUSR1;
    pid_t pid_to_send;
    int mode;
    if(argc != 4){
        perror("Incorrect number of arguments. Expected PID of catcher, number of signals to send and mode\n");
        return -1;
    }
    if(sscanf(argv[1], "%d", &pid_to_send) != 1){
        perror("Incorrect first argument. Expected PID of catcher (positive int)\n");
        return -1;
    }
    if(sscanf(argv[2], "%d", &count) != 1){
        perror("Incorrect second argument. Expected number of signals to send (positive int)\n");
        return -1;
    }
    if(sscanf(argv[3], "%d", &mode) != 1){
        perror("Incorrect third argument. Expected mode of sender (0, 1 or 2)\n");
        return -1;
    }
    if(mode == 0) {
        for (int i = 0; i < count; i++) {
            kill(pid_to_send, SIGUSR1);
        }
        sleep(1);
        kill(pid_to_send, SIGUSR2);
    }
    else if(mode == 1){
        union sigval val;
        for (int i = 0; i < count; i++) {
            sigqueue(pid_to_send, SIGUSR1, val);
        }
        sleep(1);
        sigqueue(pid_to_send, SIGUSR2, val);
    }
    else if(mode == 2) {
        for (int i = 0; i < count; i++) {
            kill(pid_to_send, SIGRTMIN + 1);
        }
        sleep(1);
        kill(pid_to_send, SIGRTMIN + 2);
    }

    while(1){

    }
    return 0;
}