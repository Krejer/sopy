#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>
#include<signal.h>

#define ERR(source) \
    (fprintf(stderr, "%s:%d\n", __FILE__, __LINE__), perror(source), kill(0, SIGKILL), exit(EXIT_FAILURE))

volatile sig_atomic_t last_signal = 0;

//int sigsuspend(const sigset_t *sigmask); // blokuje wszystkie sygnaly i czeka az dojdzie sygnal, ktory jest w sigmask
// mozna uzywać pause, aby czekać, ale wtedy cały program się zamraza
// pid_t getppid(void); - dostaje pid rodzica
// int sigprocmask(int how, const sigset_t *set, sigset_t *oldset); - zarządzanie maską sygnałow biezącego procesu, how - sposob modyfikacji maski 
// int sigaddset(sigset_t *set, int signo); - dodaje sygnał signo do signal set 
// int sigemptyset(sigset_t *set); - inicjalizuje pusty signal set

void sethandler(void (*f)(int), int sigNo)
{
    struct sigaction act;
    memset(&act, 0, sizeof(struct sigaction));
    act.sa_handler = f;
    if (-1 == sigaction(sigNo, &act, NULL))
        ERR("sigaction");
}

void sig_handler(int sig) { last_signal = sig; }

void sigchld_handler(int sig)
{
    pid_t pid;
    for (;;)
    {
        pid = waitpid(0, NULL, WNOHANG);
        if (pid == 0) // zaden proces sie jeszcze nie zakonczyl, wiec nie mamy na co czekac
            return;
        if (pid <= 0)
        {
            if (errno == ECHILD)
                return;
            ERR("waitpid");
        }
    }
}

void child_work(int m, int p)
{
    int count = 0;
    struct timespec t = {0, m * 10000};
    while (1)
    {
        for (int i = 0; i < p; i++)
        {
            nanosleep(&t, NULL);
            if (kill(getppid(), SIGUSR1))
                ERR("kill");
        }
        nanosleep(&t, NULL);
        if (kill(getppid(), SIGUSR2))
            ERR("kill");
        count++;
        printf("[%d] sent %d SIGUSR2\n", getpid(), count);
    }
}

void parent_work(sigset_t oldmask)
{
    int count = 0;
    while (1)
    {
        last_signal = 0;
        while (last_signal != SIGUSR2) // z tego co rozumiem czekamy az dojdzie sygnal sigusr2 i zwiekszamy ilosc otrzymanych
            sigsuspend(&oldmask); // to jest po to, zeby program po otrzymaniu SIGUSR2 nie zakończył działania
        count++;
        printf("[PARENT] received %d SIGUSR2\n", count);
    }
}

void usage(char *name)
{
    fprintf(stderr, "USAGE: %s m  p\n", name);
    fprintf(stderr,
            "m - number of 1/1000 milliseconds between signals [1,999], "
            "i.e. one milisecond maximum\n");
    fprintf(stderr, "p - after p SIGUSR1 send one SIGUSER2  [1,999]\n");
    exit(EXIT_FAILURE);
}

int main(int argc, char **argv)
{
    int m, p;
    if (argc != 3)
        usage(argv[0]);
    m = atoi(argv[1]);
    p = atoi(argv[2]);
    if (m <= 0 || m > 999 || p <= 0 || p > 999)
        usage(argv[0]);
    sethandler(sigchld_handler, SIGCHLD);
    sethandler(sig_handler, SIGUSR1);
    sethandler(sig_handler, SIGUSR2);
    sigset_t mask, oldmask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGUSR1);
    sigaddset(&mask, SIGUSR2);
    sigprocmask(SIG_BLOCK, &mask, &oldmask); // wszystkie sygnały odblokowane poza SIGUSR1 i SIGUSR2, oldmask przechowuje poprzedni stan, zeby moc potem przywrocic
    pid_t pid;
    if ((pid = fork()) < 0)
        ERR("fork");
    if (0 == pid)
        child_work(m, p);
    else
    {
        parent_work(oldmask);
        while (wait(NULL) > 0)
            ;
    }
    sigprocmask(SIG_UNBLOCK, &mask, NULL);
    return EXIT_SUCCESS;
}
