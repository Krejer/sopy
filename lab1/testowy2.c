#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include<unistd.h>
#include<string.h>
#include<ftw.h>

#define ERR(source) (perror(source), fprintf(stderr, "%s:%d\n", __FILE__, __LINE__), exit(EXIT_FAILURE))

#define Max 10200

const char * ext = NULL;
int depth = 1;
extern char ** environ;
int czy = 0;


int walk(const char *name, const struct stat *s, int type, struct FTW *f)
{
    switch (type)
    {
        if(f->level > depth) return 0;
        case FTW_F:
        if(strstr(name, ext) != NULL)
        {
            if(czy == 1)
            {
                char * env = getenv("L1_OUTPUTFILE");
                if(!env) czy = 0;
                if(czy == 1) 
                {
                    char new[Max];
                    snprintf(new, Max, "%s: %d\n", name, (int)s->st_size);
                    if(setenv(env, new, 1) != 0) ERR("setenv");
                }
            }
            if(czy == 0) fprintf(stdout, "%s: %d\n", name, (int)s->st_size);
            break; 
        }
        default:
            break;
    }
    return 0;
}

int main(int argc, char ** argv)
{
    if(argc < 2) ERR("Too few arguments");
    int c;
    //DIR * dir_name = NULL;
    //struct dirent *dp;
    //struct stat filestat;
    char * nazwa = NULL;
    // char * ext = NULL;
    // int depth = 1;
    FILE * output = stdout;

    while((c = getopt(argc, argv, "p:e:d:o")) != -1)
    {
        switch(c)
        {
            case 'e':
                if(ext != NULL) ERR("Too many extensions");
                ext = optarg;
                break;
            case 'p': 
                break;
            case 'o':
                czy = 1;
                break;
            case 'd':
                depth = atol(optarg);
                break;
            case '?':
            default:
                break;
        }
    }

    optind = 1;

    while((c = getopt(argc, argv, "p:e:d:o")) != -1)
    {
        switch(c)
        {
            case 'p':
            nazwa = optarg;
            if(!(nftw(nazwa, walk, Max, FTW_PHYS) == 0)) fprintf(output, "acces denied\n"); // tutaj Max oznacza maksymalną głębokość przeszukania drzewa, nie trzeba chyba tego level jak wyzej robic
                break;
            case 'e':
                break;
            case 'o':
                break;
            case 'd':
                break;
            case '?':
            default:
                ERR("cos nie działa es");
        }
    }
    int index = 0;
    while (environ[index])
        printf("%s\n", environ[index++]);
    return EXIT_SUCCESS;
}



            // if(depth == 1)
            // {
            //     char path[Max];
            //     if(getcwd(path, Max) == NULL) ERR("getcwd");
            //     if(chdir(nazwa)) ERR("chdir");
            //     if((dir_name = opendir(nazwa)) == NULL) ERR("opendir");
            //     fprintf(output, "Path: %s\n", nazwa);
            //     while((dp = readdir(dir_name)) != NULL)
            //     {
            //         if(lstat(dp->d_name, &filestat) == -1) ERR("lstat"); // lstat działa na katalogu, w ktorym obecnie sie znajduje
            //         if(strstr(dp->d_name, ext) != NULL)
            //             fprintf(output, "%s: %d\n",  dp->d_name, (int)filestat.st_size);
            //     }
            //     if(chdir(path)) ERR("chdir");
            //     if(closedir(dir_name)) ERR("closedir");
            // }
            // else