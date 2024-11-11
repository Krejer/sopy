#define _XOPEN_SOURCE 500
#define _GNU_SOURCE
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <ftw.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#define ERR(source) (perror(source), fprintf(stderr, "%s:%d\n", __FILE__, __LINE__), exit(EXIT_FAILURE))

#define Max 256

#define TEMP_FAILURE_RETRY(expression)           \
    ({                                           \
        ssize_t _result;                         \
        do {                                     \
            _result = (expression);              \
        } while (_result == -1 && errno == EINTR); \
        _result;                                 \
    })

ssize_t bulk_read(int fd, char *buf, size_t count)
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

ssize_t bulk_write(int fd, char *buf, size_t count)
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


void show_stage2(const char *const path, const struct stat *const stat_buf)
{
    int is_file = 0;
    int is_dir = 0;
    if(S_ISREG(stat_buf->st_mode)) is_file = 1;
    if(S_ISDIR(stat_buf->st_mode)) is_dir = 1;
    if(is_dir == 0 && is_file == 0) ERR("Unknown object");

    if(is_file)
    {
        FILE * s1;
        if((s1 = fopen(path, "r")) == NULL) ERR("fopen");
        char ch;
        while((ch = fgetc(s1)) != EOF) printf("%c", ch);
        printf("\n");
        if(fclose(s1)) ERR("fclose");
    }
    if(is_dir)
    {
        char cwd[Max]; // ustalamy sobie jakąś wielkość maksymalną
        if(getcwd(cwd, Max) == NULL) ERR("getcwd"); // getcwd zapisuje obecny katalog do path
        DIR *dirp; 
        struct dirent *dp;
        struct stat filestat;
        if((dirp = opendir(path)) == NULL) ERR("open dir");
        if(chdir(path)) ERR("chdir");
        while((dp = readdir(dirp)) != NULL)
        {
            if((lstat(dp->d_name, &filestat))) ERR("lstat");
            printf("%s\n", dp->d_name);
        }
        if(chdir(cwd)) ERR("cwd");
        if(closedir(dirp)) ERR("close dir");
    }

}

void write_stage3(const char *const path, const struct stat *const stat_buf)
{
    const int fd_1 = open(path, O_RDWR | O_APPEND);
    if(fd_1 == -1) ERR("open");

    char buf[Max];
    ssize_t read_size;

    while ((read_size = bulk_read(fd_1, buf, Max)) > 0)
    {
        if(write(STDOUT_FILENO, buf, read_size) < read_size)
        {
            close(fd_1);
            ERR("write");
        }
    }
    if(read_size < 0) 
    {
        close(fd_1);
        ERR("bulk_read");
    }

    while(fgets(buf, Max, stdin) != NULL)
    {
        if(strcmp(buf, "\n") == 0) break;

        ssize_t bytes_written = bulk_write(fd_1, buf, strlen(buf));
        if(bytes_written < 0)
        {
            close(fd_1);
            ERR("bulk_write");
        }
    }

    if(close(fd_1) == -1) ERR("close");
}

int walk(const char *name, const struct stat *s, int type, struct FTW *f)
{
    switch (type)
    {
        case FTW_DNR:
        case FTW_D:
            fprintf(stdout, "Katalog: %s\n", name);
            break;
        case FTW_F:
            fprintf(stdout, "Plik: %s\n", name);
            break;
        default:
            fprintf(stdout, "Nieznany obiekt: %s", name);
            break;
    }
    return 0; // tu musi zostać zwrocone 0
}

void walk_stage4(const char *const path, const struct stat *const stat_buf)
{
    if (nftw(path, walk, Max, FTW_PHYS) != 0) ERR("nftw");
}


int interface_stage1()
{
    char path[Max];
    struct stat filestat;

    printf("1. show\n2. write\n3. walk\n4. exit\n");

    if(fgets(path, sizeof(path), stdin) == NULL) ERR("fgets");
    
    char x = path[0];
    if(x == '4') return 0;
    if(x != '1' && x != '2' && x !='3') ERR("Inlvalid input");

    printf("Enter file path: ");
    // if((fgets(path, sizeof(path), stdin)) == NULL) ERR("fgets");

    // path[strlen(path) - 1] = '\0'; // fgets wczytało new line character, trzeba go usunąć i zamienić na '\0'

    int ch, i = 0; //- Imo lepsze podejście, nie trzeba podmieniac ostatniego znaku
    while ((ch = getchar()) != '\n' && ch != EOF && i < sizeof(path) - 1) {
        path[i++] = ch;
    }
    path[i] = '\0';


    if(stat(path, &filestat) == -1) ERR("stat"); // tutaj to robimy, zeby potem wewnatrz funkcji wszystkich musiec statować
    
    switch (x)
    {
    case  '1':
        show_stage2(path, &filestat);
        return 1;
    case '2':
        write_stage3(path, &filestat);
        return 1;
    case '3':
        walk_stage4(path, &filestat);
        return 1;
    default:
        return 1;
    }
    return 1;
}


int main()
{
    while (interface_stage1())
        ;
    return EXIT_SUCCESS;
}
