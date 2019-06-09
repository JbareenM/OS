#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/uio.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/fcntl.h>

pid_t pidInd[5];
int ind = 0;
pid_t ppid;

void sigCatcher(int signo){
    printf("PID %d caught one\n", getpid());
    kill(pidInd[ind], SIGINT);
    ind++;
    exit(0);
}
void clear(int signo){
    for (int i = 0; i < 5; i++){
        kill(pidInd[i], SIGTERM);
        printf("Process %d is dead\n", pidInd[i]);
        sleep(1);
    }
}
int main(){
    ppid = getpid();
    for (int i = 0; i < 5; i++){
        pid_t pid = fork();
        if (pid < 0){
            fprintf(stderr, "Failed at creating a new process.");
            return 1;
        }
        else if (pid > 0){
            signal(SIGINT, clear);
            pidInd[i] = pid;
        }
        else{
            signal(SIGINT, sigCatcher);
            printf("PID %d ready\n", getpid());
            pause();
        }
    }
    sleep(1);
    kill(pidInd[ind], SIGINT);
    pause();
    exit(0);
}
