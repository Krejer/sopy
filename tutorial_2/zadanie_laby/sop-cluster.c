#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>
#include<signal.h>


#define ERR(source) \
    (perror(source), fprintf(stderr, "%s:%d\n", __FILE__, __LINE__), kill(0, SIGKILL), exit(EXIT_FAILURE))

#define UNUSED(x) (void)(x)

volatile sig_atomic_t last_signal = 0;

void sig_handler(int sig){last_signal = sig;}

void set_handler(void (*f)(int), int sigNo)
{
    struct sigaction act;
    memset(&act, 0, sizeof(struct sigaction));
    act.sa_handler = f;
    if (-1 == sigaction(sigNo, &act, NULL))
        ERR("sigaction");
}

void child_work()
{
    last_signal = 0;
    int count = 0;
    srand(time(NULL) * getpid());
    int m = rand() % 2 + 1;
    sigset_t mask, oldmask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGUSR1);
    sigprocmask(SIG_BLOCK, &mask, &oldmask);
    pid_t pid = getpid();
    struct timespec t = {0, m * 100000000};
    while(last_signal != SIGUSR1) sigsuspend(&oldmask);
    count++;
    while(1)
    {
        nanosleep(&t, NULL);
        count++;
        int c = count;
        if(fprintf(stdout, "PID: %d, licznik: %d\n", pid, c) < 0) ERR("fprintf");
    }
    //exit(EXIT_SUCCESS);
}

void parent_work(pid_t pid, int sign)
{
    //sleep(1);
    if(kill(pid, sign) < 0) ERR("kill");
}



int main(int argc, char* argv[])
{
    if(argc != 2) ERR(argv[0]);

    int n = atoi(argv[1]);
    pid_t pid;
    set_handler(sig_handler, SIGUSR1);
    if((pid = fork()) < 0) ERR("fork");
    if(0 == pid) child_work();
    else parent_work(pid, SIGUSR1);
    for(int i = 1; i < n; i++)
    {
        if((pid = fork()) < 0) ERR("fork");
        if(0 == pid) child_work();
    }
    while (wait(NULL) > 0)
        ;

    return EXIT_SUCCESS;
}