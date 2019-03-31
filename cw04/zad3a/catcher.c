#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

int signal_count = 0;
struct sigaction act;
struct sigaction act_rt;

void catchSIGUSR1(int sig, siginfo_t* info, void* context){
    sigaction(SIGUSR1, &act, NULL);
    signal_count++;
}

void catchSIGUSR2(int sig, siginfo_t* info, void* context){
    printf("[CATCHER] Signals caught: %d\n", signal_count);
    for(int i = 0; i < signal_count; i++){
        kill(info->si_pid, SIGUSR1);
    }
    sleep(1);
    kill(info->si_pid, SIGUSR2);
    exit(0);
}

void catchSIGRT1(int sig, siginfo_t* info, void* context){
    sigaction(SIGRTMIN + 1, &act_rt, NULL);
    signal_count++;
}

void catchSIGRT2(int sig, siginfo_t* info, void* context){
    printf("[CATCHER] Signals caught: %d\n", signal_count);
    for(int i = 0; i < signal_count; i++){
        kill(info->si_pid, SIGUSR1);
    }
    sleep(1);
    kill(info->si_pid, SIGUSR2);
    exit(0);
}


int main(int argc, char* argv[]){
    sigset_t blocked;
    sigfillset(&blocked);
    sigdelset(&blocked, SIGUSR1);
    sigdelset(&blocked, SIGUSR2);
    sigdelset(&blocked, SIGRTMIN + 1);
    sigdelset(&blocked, SIGRTMIN + 2);
    sigprocmask(SIG_SETMASK, &blocked, NULL);

    pid_t pid = getpid();
    printf("PID OF CATCHER: %d\n", pid);
    act.sa_sigaction = catchSIGUSR1;
    sigemptyset(&act.sa_mask);
    act.sa_flags = SA_SIGINFO;
    sigaction(SIGUSR1, &act, NULL);
    act.sa_sigaction = catchSIGUSR2;
    sigaction(SIGUSR2, &act, NULL);
    act.sa_sigaction = catchSIGUSR1;

    act_rt.sa_sigaction = catchSIGRT1;
    sigemptyset(&act_rt.sa_mask);
    act_rt.sa_flags = SA_SIGINFO;
    sigaction(SIGRTMIN + 1, &act_rt, NULL);
    act_rt.sa_sigaction = catchSIGRT2;
    sigaction(SIGRTMIN + 2, &act_rt, NULL);
    act_rt.sa_sigaction = catchSIGUSR1;
    while(1){

    }
    return 0;
}