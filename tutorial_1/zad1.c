#include <dirent.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include<unistd.h>

#define ERR(source) (perror(source), fprintf(stderr, "%s:%d\n", __FILE__, __LINE__), exit(EXIT_FAILURE))

// DIR * opendir(DIR *dirname); // zwraca wskaźnik do katalogu jeśli się powiedzie, wpp null, errno ustawione na null
// closedir(DIR *dirname); zwraca 0, gdy sie powiedzie, -1 gdy sie nie powiedzie
// struct dirent *readdir(DIR *dirp); zwraca structa, lub nulla jeśli dojdzie do końca katalogu, czyli będzie trzeba whilem,
// ten struct dirent posiada dwa pola - int inode - numer inode pliku, char nazwa[] - nazwa pliku
// int lstat(const char *restrict path, struct stat *restrict buf); lstat to stat tylko nie jesli path odnosi sie do symbolic linka, wtedy lstat zwroci info o linku
// , podczas gdy stat info o pliku, ktorego link dotyczy, lstat i stat zwraca 0 jesli sie udalo, wpp -1
// int stat(const char *restrict path, struct stat *restrict buf); argument path wskazuje na sciezke do pliku, ktory chcemy odczytac, do bufa sa przesylane info o pliku
// sys/stat.h — data returned by the stat() function
// char * getcwd(char * buf, size_t size); w buf getcwd zapisze nazwe obecnego katalogu, getcwd zwraca buf, jeśli się nie powiedzie zwraca null, ustawia odpowiednio errno,
// buf musi byc wczesniej zaalokowana tablica, WAZNE - buf musi byc bezwzgledna sciezka, zaczynajaca sie od /... - ona jest jednoznaczna
// int chdir(const char *path); podajemy po prostu w path nazwe katalogu na ktory chcemy zmienic, 0 jeśli sie powiedzie, -1 wpp. path nie musi byc bezwzgledny, moze byc wzgledna - 
// odnosi sie do katalogu, w ktorym obecnie sie znajdujemy

void scan_dir()
{
    DIR *dirp; // tworze zmienna typu DIR * zeby moc korzystac z opendir
    struct dirent *dp; // potrzebuje to, zeby korzystac z readdir
    struct stat filestat; // potrzebuje to, zeby korzystac z lstat
    int dirs = 0, files = 0, links = 0, other = 0;
    if ((dirp = opendir(".")) == NULL) // sprawdzam czy sie otworzył, . to katalog obecny, roboczy
        ERR("opendir");
    while ((dp = readdir(dirp)) != NULL)
    {
        if (lstat(dp->d_name, &filestat)) // lstat zapisuje info o pliku w filestat
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

    if (errno != 0) // sprawdzam, czy readdir sie ladnie zakonczyl, czy jakis blad sie pojawil przed readdira
        ERR("readdir");
    if (closedir(dirp))
        ERR("closedir");
    printf("Files: %d, Dirs: %d, Links: %d, Other: %d\n", files, dirs, links, other);
}
// komenda ln -s zad1.c prog_link.c tworzy symlink do zad1.c(powstaje nowy plik)
int main()
{
    scan_dir();
    return EXIT_SUCCESS;
}