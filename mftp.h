#ifndef MFTP_H
#define MFTP_H
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <sys/wait.h>
#include <libgen.h>

int split_space(char *src, char *dest1, char *dest2)
{
    int len = strlen(src);

    for (int i = 0; i < len; i++) //"remove" new lines
    {
        if (src[i] == '\n')
        {
            len--;
            src[i] = 0;
        }
    }

    char *space_ptr = strchr(src, ' ');
    int posn = -1;

    if (space_ptr != NULL)
    {
        posn = space_ptr - src;
        printf("space_ptr: %s\n", space_ptr);
        printf("posn: %d\n", posn);
        printf("len: %d\n", len);
        strncpy(dest1, src, posn);                 //gets the first word (command)
        strncpy(dest2, space_ptr + 1, len - posn); //gets the path
        dest1[posn] = 0;
        dest2[len - posn] = 0;
        printf("@%s@%s\n", dest1, dest2);
        return posn;
    }
    else
    {
        strcpy(dest1, src);
        dest1[len] = 0;
        *dest2 = 0;
        return strlen(src);
    }
}

int fun_cd(char *path, char *new_path) //gets cd working
{
    char s[256];
    chdir(path);
    strcpy(new_path, getcwd(s, 256));
}

int fun_ls() //gets ls working
{
    int pid = fork();

    if (pid != 0) // parent
    {
        waitpid(-1, NULL, 0); // zombie process
    }
    else // child
    {

        int fd[2];
        pipe(fd);
        int reader = fd[0];
        int writer = fd[1];

        if (fork()) // parent
        {
            close(reader);
            close(1);
            dup(writer);
            close(writer);
            execlp("ls", "ls", "-l", NULL); // runs command
            exit(0);
        }
        else // child
        {
            close(writer);
            close(0);
            dup(reader);
            close(reader);
            execlp("more", "more", "-20", NULL); // displays first 20 lines
            exit(1);
        }
    }
}

#endif