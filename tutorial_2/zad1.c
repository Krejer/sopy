#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>
#include<signal.h>
#define ERR(source) \
    (fprintf(stderr, "%s:%d\n", __FILE__, __LINE__), perror(source), kill(0, SIGKILL), exit(EXIT_FAILURE))


// fork tworzy nowy proces - pid_t fork(void) - proces dziecko ma ukinalny pid. id dziecka nie jest takie same jak zadne inne z grupy, ppid rodzica, ktory stworzyl. Ogolnie
// proces dziecko dziedzicy po starym otwarte deskryptory czy pamiec(prawie wszystko), fork zwraca pid dziecka, ale ogolnie sam fork zwraca 0 jesli sie powiodlo, -1 jesli nie
// pid_t getpid(void) - zwraca id procesu
// pid_t wait(int * static_loc) - czeka dopoki proces dziecko sie nie zatrzyma, lub dopoki nie umrze
// pid_t waitpid(pid_t pid, int* stat_loc, int options), pid okresla na ktory proces, lub grupe procesow mamy czekac - >0 - konkretny proces o tym pid, -1 - czeka na dowolony proces
// potomny(wait), 0 - czeka na dowolny proces potomny z tej samej grupy procesow, <-1 - czeka na dowolny proces z grupy procesow o pidzie rownym wartości bezwzglednej
// status - status zakonczenia procesu potomnego, jest kilka dostepnych makr, options - flagi zmieniajace dzialanie funkcji

void child_work(int i)
{
    srand(time(NULL) % getpid());
    int t = 5 + rand() % (10 - 5 + i);
    sleep(t);
    printf("Process with PID: %d terminates\n", getpid());
}

void create_children(int n)
{
    pid_t s;
    for(n--; n >= 0; n--)
    {
        if((s = fork()) < 0) ERR("fork");
        if(s == 0)
        {
            child_work(n);
            exit(EXIT_SUCCESS); // to musi byc, wpp proces potomny bedzie tworzyl kolejny proces potomny
        }

    }
}

void usage(char * name)
{
    fprintf(stderr, "USAGE: %s 0<n\n", name);
    exit(EXIT_FAILURE);
}


int main(int argc, char ** argv)
{
    if(argc < 2) usage(argv[0]);
    int n = atoi(argv[1]);
    if(n <= 0) usage(argv[0]);
    create_children(n);
    while(n > 0)
    {
        sleep(3);
        pid_t pid;
        for(;;)
        {
            pid = waitpid(0, NULL, WNOHANG); // jesli jeszcze id nie jest dostepne to nie zakonczy sie dzialanie, tylko poczeka
            if(pid > 0) n--;
            if(0 == pid) break; // cwhilowy brak zakonczonych potmkow(flaga WNOHANG)
            if(0 >= pid)
            {
                if(ECHILD == errno) break;
                ERR("waitpid");
            }
        }
        printf("Liczba procesow rodzica: %d\n", n);
    }

    return EXIT_SUCCESS;
}