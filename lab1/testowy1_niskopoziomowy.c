#define _GNU_SOURCE
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <errno.h>
#include <sys/stat.h>


#define TEMP_FAILURE_RETRY(expression)           \
    ({                                           \
        ssize_t _result;                         \
        do {                                     \
            _result = (expression);              \
        } while (_result == -1 && errno == EINTR); \
        _result;                                 \
    })


#define ERR(source) (perror(source), fprintf(stderr, "%s:%d\n", __FILE__, __LINE__), exit(EXIT_FAILURE))

#define Max 100



int main(int argc, char ** argv)
{
    if(argc < 2) ERR("Too few arguments");
    DIR * dir_name = NULL;
    //FILE * plik;
    struct dirent *dp;
    struct stat filestat;
    char * nazwa = NULL;
    int c;
    while((c = getopt(argc, argv, "p:o:")) != -1)
    {
        switch(c)
        {
            case 'o':
            if(nazwa != NULL) ERR("Too many arguments");
                nazwa = optarg;
                break;
            case 'p':
                break;
            case '?':
            default:
                break;
        }
    }
    optind = 1;
    const int fd_open = open(nazwa, O_WRONLY | O_CREAT, 0777);
    //if((plik = fopen(nazwa, "w+")) == NULL) ERR("fopen");
    while((c = getopt(argc, argv, "o:p:")) != -1)
    {
        switch(c)
        {
            case 'o':
                break;
            case 'p':
                nazwa = optarg;
                char path[Max];
                if(getcwd(path, Max) == NULL) ERR("getcwd");
                if(chdir(nazwa)) ERR("chdir");
                if((dir_name = opendir(nazwa)) == NULL) 
                    ERR("readdir");
                while ((dp = readdir(dir_name)) != NULL)
                {
                    if(lstat(dp->d_name, &filestat) == -1) ERR("lstat"); // lstat działa na katalogu, w ktorym obecnie sie znajduje
                    char buffer[Max];
                    strcpy(buffer, dp->d_name);
                    int len = strlen(buffer);
                    write(fd_open, buffer, len);
                    //fprintf(plik, "%s %d\n", dp->d_name, (int)filestat.st_size);
                }
                if(chdir(path)) ERR("chdir");
                if((closedir(dir_name))) 
                    ERR("closedir");
                break;
            case '?':
            default:
                ERR("cos nie dziala");
        }
    }

    //if((fclose(plik))) ERR("fclose");
    if(close(fd_open) == -1) ERR("close");
    return EXIT_SUCCESS;
}

