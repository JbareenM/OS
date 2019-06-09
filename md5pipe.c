#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/uio.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <sys/fcntl.h>
#include <sys/wait.h>
#include "md5.h"
using std::string;

bool _encrypt = false;
char secret[33];
void _signal(int signo){
    if (strlen(secret) == 32) _encrypt = true;
}

int main(int argc, char *argv[]){
    int _childMsg[2];
    int _parentMsg[2];
    pid_t pid;
    if (pipe(_childMsg) == -1 || pipe(_parentMsg) == -1){
        fprintf(stderr, "Failed!");
        return 1;
    }
    char input_str[20];
    printf("plain text: ");
    scanf("%s", input_str);
    pid = fork();
    if (pid < 0){
        fprintf(stderr, "Failed!");
        return 1;
    }
    else if (pid > 0){
        signal(SIGINT, _signal);
        close(_childMsg[0]);
        write(_childMsg[1], input_str, strlen(input_str) + 1);
        close(_childMsg[1]);
        close(_parentMsg[1]);
        read(_parentMsg[0], secret, 33);
        close(_parentMsg[0]);
        if (_encrypt){
            printf("encrypted by process %d : %s\n", pid, secret);
            kill(pid, SIGTERM);
        }
    }
    else{
        close(_childMsg[1]);
        char Msg[100];
        read(_childMsg[0], Msg, 100);
        close(_childMsg[0]);
        string ans = md5(Msg);
        close(_parentMsg[0]);
        write(_parentMsg[1], ans.c_str(), ans.length() + 1);
        close(_parentMsg[1]);
        kill(getppid(), SIGINT);
    }
    return 0;
}
