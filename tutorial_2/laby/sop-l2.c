#define _GNU_SOURCE
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
#include<ctype.h>

#define ERR(source) (perror(source), fprintf(stderr, "%s:%d\n", __FILE__, __LINE__), kill(0, SIGKILL), exit(EXIT_FAILURE))
#define MAX 256

volatile sig_atomic_t last_signal = 0;

volatile sig_atomic_t sig_int_received = 0;

#define TEMP_FAILURE_RETRY(expression) \
  (__extension__({ \
    long int __result; \
    do { \
      __result = (long int) (expression); \
    } while (__result == -1L && errno == EINTR); \
    __result; \
  }))


void sighandler(int sig){last_signal = sig;}

void sig_int_parent_handler(int sig) {
    struct sigaction sa_ignore, sa_restore;

    // Ignoruj SIGINT
    sa_ignore.sa_handler = SIG_IGN;
    sigemptyset(&sa_ignore.sa_mask);
    sa_ignore.sa_flags = 0;
    if (sigaction(SIGINT, &sa_ignore, &sa_restore) < 0) ERR("sigaction");

    // Wyślij SIGINT do dzieci
    if (kill(0, sig) < 0) ERR("kill");

    // Przywróć obsługę SIGINT
    if (sigaction(SIGINT, &sa_restore, NULL) < 0) ERR("sigaction");
}


ssize_t bulk_read(int fd, char* buf, size_t count)
{
    ssize_t c;
    ssize_t len = 0;
    do
    {
        c = TEMP_FAILURE_RETRY(read(fd, buf, count));
        if (c < 0)
            return c;
        if (c == 0)
            return len;  // EOF
        buf += c;
        len += c;
        count -= c;
    } while (count > 0);
    return len;
}

ssize_t bulk_write(int fd, char* buf, size_t count)
{
    ssize_t c;
    ssize_t len = 0;
    do
    {
        c = TEMP_FAILURE_RETRY(write(fd, buf, count));
        if (c < 0)
            return c;
        buf += c;
        len += c;
        count -= c;
    } while (count > 0);
    return len;
}

void sethandler(void (*f)(int), int sigNo)
{
    struct sigaction act;
    memset(&act, 0, sizeof(struct sigaction));
    act.sa_handler = f;
    if (-1 == sigaction(sigNo, &act, NULL))
        ERR("sigaction");
}

void usage(int argc, char* argv[])
{
    printf("%s n f \n", argv[0]);
    printf("\tf - file to be processed\n");
    printf("\t0 < n < 10 - number of child processes\n");
    exit(EXIT_FAILURE);
}

void child_work(char * text, int i, char * filename)
{
    sethandler(sighandler, SIGINT);
    char name[MAX];
    snprintf(name, sizeof(name), "%s-%d", filename, i);
    int out;
    ssize_t count;
    int licznik = 1, index = 0, czy = 0;
    if ((out = open(name, O_WRONLY | O_CREAT | O_TRUNC | O_APPEND, 0777)) < 0) ERR("open");
    pid_t pid = getpid();
    sigset_t empty;
    sigemptyset(&empty);
    while(last_signal != SIGUSR1 && last_signal != SIGINT) sigsuspend(&empty);
    if(last_signal == SIGINT)
    {
        if(close(out) < 0) ERR("close");
        exit(EXIT_SUCCESS);
    }
    while(index < strlen(text) && czy == 0)
    {
        if((text[index] >= 65 && text[index] <= 90) || (text[index] >= 97 && text[index] <= 122))
        {
            if(licznik == 1)
            {
                if(text[index] <= 90) text[index] = tolower(text[index]);
                else text[index] = toupper(text[index]);
                licznik = 0;
                struct timespec t = {0, 250000000}, rem;
                while(nanosleep(&t, &rem) == -1)
                {
                    if(errno == EINTR)
                    {
                        if(last_signal == SIGINT && czy == 0)
                        {
                            char buf = text[index];
                            if((count = write(out, &buf, 1)) < 0) ERR("write");
                            if(close(out) < 0) ERR("close");
                            exit(EXIT_SUCCESS);
                        }
                    }
                    t = rem;
                }
                char buf = text[index];
                if((count = write(out, &buf, 1)) < 0) ERR("write");
            }
            else licznik = 1;

        }
        index++;
    }
    if(close(out)) ERR("close");
    if(fprintf(stdout, "PID: %d\ntekst: %s\n", pid, text) < 0) ERR("fprintf");
}

int main(int argc, char* argv[])
{
    if(argc != 3) usage(argc, argv);
    int in;
    char * filename = argv[1];
    struct stat sb;
    if(stat(filename, &sb) == -1) ERR("stat");
    off_t len = sb.st_size;

    sethandler(sighandler, SIGUSR1); 
    sethandler(sig_int_parent_handler, SIGINT);

    int n = atoi(argv[2]);
    if(n <= 0 || n > 10) ERR("Invalid input");
    int dlugosc = len/n;
    int missing = len - (n-1) * dlugosc;
    int ile = 0;

    pid_t pid;
    for(int i = 0; i < n; i++)
    {
        if(i == n - 1) dlugosc = missing;
        char tekst[dlugosc + 1];
        if((pid = fork()) < 0) ERR("fork");
        if(0 == pid)
        {
            if((in = open(filename, O_RDONLY)) < 0) ERR("open");
            if(lseek(in, ile, SEEK_SET) == -1)
            {
                ERR("lseek");
                close(in);
            }
            ssize_t count = bulk_read(in, tekst, dlugosc);
            if(count < 0) ERR("bulkread");
            tekst[count] = '\0';
            close(in);
            child_work(tekst, i+1, filename);
            exit(EXIT_SUCCESS);    
        }
        ile += dlugosc;
    }
    if(kill(0, SIGUSR1) < 0) ERR("kill");
    while(wait(NULL) > 0);
    return EXIT_SUCCESS;
}
