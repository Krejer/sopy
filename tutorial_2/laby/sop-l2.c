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

#define ERR(source) (perror(source), fprintf(stderr, "%s:%d\n", __FILE__, __LINE__), kill(0, SIGKILL), exit(EXIT_FAILURE))
#define MAX 1000

volatile sig_atomic_t last_signal = 0;

#define TEMP_FAILURE_RETRY(expression) \
  (__extension__({ \
    long int __result; \
    do { \
      __result = (long int) (expression); \
    } while (__result == -1L && errno == EINTR); \
    __result; \
  }))


void sighandler(int sig){last_signal = sig;}

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

void child_work(char * text)
{
    sigset_t empty;
    sigemptyset(&empty);
    pid_t pid = getpid();
    while(last_signal != SIGUSR1) sigsuspend(&empty);
    if(fprintf(stdout, "PID: %d\ntekst: %s\n", pid, text) < 0) ERR("fprintf");
}

int main(int argc, char* argv[])
{
    if(argc != 3) usage(argc, argv);
    int in;
    const char * filename = argv[1];
    struct stat sb;
    if(stat(filename, &sb) == -1) ERR("stat");
    off_t len = sb.st_size;

    sethandler(sighandler, SIGUSR1); 

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
            ssize_t count;
            if((count = read(in, tekst, dlugosc)) < 0) ERR("read");
            tekst[count] = '\0';
            close(in);
            child_work(tekst);
            exit(EXIT_SUCCESS);    
        }
        ile += dlugosc;
    }
    if(kill(0, SIGUSR1) < 0) ERR("kill");
    while(wait(NULL) > 0);
    return EXIT_SUCCESS;
}
