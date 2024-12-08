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

// int sigaction(int sig, const struct sigaction, sigaction * restrict act) - zwraca 0 jesli wszystko git, -1 wpp - ustala co sygnal sig od teraz bedzie robil(zmienia dzialanie)
// int nanosleep(const struct timespec *rqtp, struct timespec *rmtp); - sprawia, ze obecny watek zostaje zatrzymany, dopoki sygnal dojdzie, lub rqtp(czas upłynie)
// unsigned alarm(unsigned seconds); // spowoduje wywołanie sygnału SIGALARM po upływie seconds normalnego czasu
// void* memset(void* ptr, int value, size_t num); - ustawia blok pamięci na jakąś konkretną wartość - value, num jest wielkością bloku
// int kill(pid_t pid, int sig); - wysyła sygnał do procesu lub grupy procesow(specified by pid), 0 jeśli wszystko git, -1 wpp

volatile sig_atomic_t last_signal = 0; // volatile mowi kompilatorowi, ze jej zawartosc moze sie zmienic w kazdej chwili nawet jesli kod jej nie zmienia(sygnal przyjdzie)
//ta zmienna globalna jest lokalna dla kazdego procesu, czyli kazde dziecko moze miec inna wartosc zmiennej globalnej

void sethandler(void (*f)(int), int sigNo)
{
    struct sigaction act; // to jest potrzebne do pozniejszego uzycia sigaction
    memset(&act, 0, sizeof(struct sigaction)); // ustawiamy, zeby nie miec smieci
    act.sa_handler = f; // funkcja f bedzie wywolana w odpowiedzi na odpowiedni sygnal, czyli tu ustalamy co sie stanie tak jakby po otrzymaniu sygnalu
    if (-1 == sigaction(sigNo, &act, NULL)) // nie musze podawac potem argumentu do sig_handler bo tutaj kompilator sam go poda
        ERR("sigaction");
}

void sig_handler(int sig) // to jest nasz function handle, to bedzie sie wykonywalo po ustawieniu sig
{
    printf("[%d] received signal %d\n", getpid(), sig); // proces otrzymal jakis tam sygnal
    last_signal = sig; // no i teraz sie wyjasnia po co zmienna globalna volatile
}

void sigchld_handler(int sig)
{
    pid_t pid;
    for (;;)
    {
        pid = waitpid(0, NULL, WNOHANG);
        if (pid == 0)
            return;
        if (pid <= 0)
        {
            if (errno == ECHILD)
                return;
            ERR("waitpid");
        }
    }
}

void child_work(int l)
{
    int t, tt;
    srand(getpid());
    t = rand() % 6 + 5;
    struct timespec p = {t, 0};
    while (l-- > 0)
    {
        for (tt = t; tt > 0; tt = sleep(tt)) // dziecko zawsze bedzie spało t sekund i dopiero potem się obudzi
            ;
        if (last_signal == SIGUSR1)
            printf("Success [%d]\n", getpid());
        else
            printf("Failed [%d]\n", getpid());
    }
    printf("[%d] Terminates \n", getpid());
}

void parent_work(int k, int p, int l)
{
    struct timespec tk = {k, 0}; // k to czas w sekundach, 0 to czas w nanosekundach
    struct timespec tp = {p, 0};
    sethandler(sig_handler, SIGALRM); // SIGALRM to jest sygnał ktry informuje o upływie określonego czasu
    alarm(l * 10); // ustawiamy jakiś czas po ktorym zostanie wysłany sygnał SIGALRM
    while (last_signal != SIGALRM) // dopoki nie uplynie iles tam czasu
    {
        nanosleep(&tk, NULL); // tu musimy uzyć nanosleep, zeby nie było konfliku między sleep a alarm(sleep zwraca SIGALRM)
        // NULL jako drugi argument oznacza, ze nie interesuje nas ile nie dospał proces
        if (kill(0, SIGUSR1) < 0) // wysyla sigusr1 do wszystkich procesow z grupy, 0 wysyła tez sygnał samemu sobie(w sumie logiczne, wysyła do całej grupy)
            ERR("kill");
        nanosleep(&tp, NULL); // potem se kima
        if (kill(0, SIGUSR2) < 0) // to samo dla sigusr2
            ERR("kill");
    }
    printf("[PARENT] Terminates \n");
}   

void create_children(int n, int l)
{
    while (n-- > 0)
    {
        switch (fork())
        {
            case 0:
                sethandler(sig_handler, SIGUSR1); // ustawiamy function handle'a dla procesu, zeby wypisywal tamto coś jak otrzyma dany sygnał
                sethandler(sig_handler, SIGUSR2);
                child_work(l);
                exit(EXIT_SUCCESS);
            case -1:
                perror("Fork:");
                exit(EXIT_FAILURE);
        }
    }
}


void usage(void)
{
    fprintf(stderr, "USAGE: signals n k p l\n");
    fprintf(stderr, "n - number of children\n");
    fprintf(stderr, "k - Interval before SIGUSR1\n");
    fprintf(stderr, "p - Interval before SIGUSR2\n");
    fprintf(stderr, "l - lifetime of child in cycles\n");
    exit(EXIT_FAILURE);
}


int main(int argc, char **argv)
{
    int n, k, p, l;
    if (argc != 5)
        usage();
    n = atoi(argv[1]);
    k = atoi(argv[2]);
    p = atoi(argv[3]);
    l = atoi(argv[4]);
    if (n <= 0 || k <= 0 || p <= 0 || l <= 0)
        usage();
    sethandler(sigchld_handler, SIGCHLD);
    sethandler(SIG_IGN, SIGUSR1); // chyba chodzi o to, zeby ignorowac dopoki jakies dziecko nie powstanie, wtedy zmieniamy, zeby wypisywal, ze dziecko otrzymalo dany sygnał
    sethandler(SIG_IGN, SIGUSR2); // to jest po to, zeby proces rodzic ignorował SIGUSR1 i SIGUSR2, zeby się sam nie zabił wysłanym sygnałem
    create_children(n, l);
    parent_work(k, p, l);
    while (wait(NULL) > 0) // to sprawia, ze wszystkie procesy zombie na pewno zostaną wywaitowane
        ;
    return EXIT_SUCCESS;
}