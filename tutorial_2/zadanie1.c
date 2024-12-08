#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>
#include<signal.h>

#define ERR(source) \
    (fprintf(stderr, "%s:%d\n", __FILE__, __LINE__), perror(source), kill(0, SIGKILL), exit(EXIT_FAILURE))

volatile sig_atomic_t count = 0;
volatile sig_atomic_t last_signal = 0;

void sig_handler(int sign)
{
    last_signal = sign;
}

void sig_handler2(int sign)
{
    last_signal = sign;
    count++;
}

void set_handler(void (*f)(int), int sign)
{
    struct sigaction act;
    memset(&act, 0, sizeof(struct sigaction));
    act.sa_handler = f;
    if(-1 == sigaction(sign, &act, NULL)) ERR("sigaction");
}


void sigchld_handler(int sig)
{
    pid_t pid;
    for(;;)
    {
        pid = waitpid(0,NULL,WNOHANG);
        if(pid == 0) return;
        else if(pid < 0)
        {
            if(errno == ECHILD) return;
            ERR("waitpid");
        }
    }
}

void child_work(char name)
{
    srand(time(NULL) % getpid()); // to mi zapewnia ukinalny seed
    int s = rand() % 91 + 10;
    ssize_t count;
    char * buf;
    buf = malloc(s);
    for(int j = 0; j < s; j++) buf[j] = name;
    if(!buf) ERR("malloc");
    int i, out;
    int n = atoi(&name);
    char filename[256];
    snprintf(filename, sizeof(filename), "%d.txt", n);
    if((i = open(filename , O_WRONLY | O_CREAT | O_TRUNC | O_APPEND, 0777)) < 0) ERR("open");

    if((count = write(i, buf, s)) < 0) ERR("write");
    
    while (1)
    {
        if(last_signal == SIGUSR1)
        {
            fprintf(stderr, "PID: %d, count: %zd\n", getpid(), count);
            last_signal = 0;
        }
    }
    

    //if(fprintf(stderr, "n: %d, s: %d\n", n, s) < 0) ERR("fprintf");
    if(close(i)) ERR("close");
    free(buf);
    exit(EXIT_SUCCESS);
}

void parent_work()
{
    set_handler(SIG_IGN, SIGUSR1);
    set_handler(sig_handler, SIGALRM);
    struct timespec t = {0, 10000000};
    alarm(1);
    while(last_signal != SIGALRM)
    {
        nanosleep(&t, NULL);
        if(kill(0, SIGUSR1) < 0) ERR("kill");
    }
    //if(kill(0, SIGUSR2) < 0) ERR("kill");
}


void usage(char *name)
{
    fprintf(stderr, "USAGE: %s m  p\n", name);
    fprintf(stderr, "Wrong number, expected a number from 0 to 10");
    exit(EXIT_FAILURE);
}

int main(int argc, char ** argv)
{
    int n;
    set_handler(sigchld_handler, SIGCHLD);
    set_handler(sig_handler2, SIGUSR1);
    pid_t pid;
    for(int i = 0; i < argc - 1; i++)
    {
        int liczba  = atoi(argv[i + 1]);
        if(liczba < 0 || liczba > 9) usage(argv[0]);
        if((pid = fork()) < 0) ERR("fork");
        if(0 == pid) child_work(*argv[i + 1]);
        else
        {
            parent_work();
            while (wait(NULL) > 0)
                ;
        }
    }
    

    return EXIT_SUCCESS;
}