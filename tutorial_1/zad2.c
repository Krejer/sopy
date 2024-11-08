#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include<unistd.h>

#define MaxPath 101

#define ERR(source) (perror(source), fprintf(stderr, "%s:%d\n", __FILE__, __LINE__), exit(EXIT_FAILURE))


// to samo zadanie co zad2, ale bedzie zliczal obiekty podane we wszystkich folderach podanych jako argumenty
void scan_dir()
{
    DIR *dirp; 
    struct dirent *dp;
    struct stat filestat;
    int dirs = 0, files = 0, links = 0, other = 0;

    if ((dirp = opendir(".")) == NULL) 
        ERR("opendir");
    while ((dp = readdir(dirp)) != NULL)
    {
        if (lstat(dp->d_name, &filestat)) 
            ERR("lstat");
        if (S_ISDIR(filestat.st_mode))
            dirs++;
        else if (S_ISREG(filestat.st_mode))
            files++;
        else if (S_ISLNK(filestat.st_mode))
            links++;
        else
            other++;
    }

    if (errno != 0)
        ERR("readdir");
    if (closedir(dirp))
        ERR("closedir");
    printf("Files: %d, Dirs: %d, Links: %d, Other: %d\n", files, dirs, links, other);
}

int main(int argc, char ** argv)
{
    char path[MaxPath]; // ustalamy sobie jakąś wielkość maksymalną
    if(getcwd(path, MaxPath) == NULL) ERR("getcwd"); // getcwd zapisuje obecny katalog do path
    for(int i = 1; i < argc; i++)
    {
        if(chdir(argv[i])) ERR("chdir");
        printf("%s:\n", argv[i]);
        scan_dir();
        if(chdir(path)) ERR("chdir");
    }
    return EXIT_SUCCESS;
}