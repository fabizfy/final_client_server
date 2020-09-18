#include <time.h>
#include <signal.h>

#include "mftp.h"

#define MY_PORT_NUMBER 49999

int main(char argc, char *argv[])
{
    int listenfd;
    int option = 1;
    listenfd = socket(AF_INET, SOCK_STREAM, 0);                              //creates the "waiter" for connection
    setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option)); //let's socket to be used again

    struct sockaddr_in servAddr;
    struct sockaddr_in clientAddr;

    memset(&servAddr, 0, sizeof(servAddr)); //allocates space

    //makes the server possible to be connected
    servAddr.sin_family = AF_INET;
    servAddr.sin_port = htons(MY_PORT_NUMBER);
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY);

    int binder;
    binder = bind(listenfd, (struct sockaddr *)&servAddr, sizeof(servAddr)); ///binds the socket to the address and port number

    if (binder < 0) //checks for binder error
    {
        perror("bind");
        exit(1);
    }

    listen(listenfd, 1); //waits for the client to make a connection

    int pid;
    int counter = 0;
    int connectfd = 0;
    int length = sizeof(struct sockaddr_in);
    int flag = 0;

    while (1) //server stays up
    {

        waitpid(-1, NULL, WNOHANG); //take cares of zombie process

        connectfd = accept(listenfd, (struct sockaddr *)&clientAddr, &length); //accept connection
        counter++;
        printf("connection received # %d.\n", counter);

        if (connectfd < 0) //checks if there is a connection
        {
            fprintf(stderr, "connection error %d\n", errno);
            exit(1);
        }

        if (pid = fork()) //multiple connection
        {
            //printf("pid = fork\n");
            close(connectfd);
        }

        else
        {
            //printf("else statement\n");
            close(listenfd);
            break;
        }
    }

    char *date;
    struct hostent *hostEntry;
    char *hostName;
    char cmd[256];

    hostEntry = gethostbyaddr(&(clientAddr.sin_addr), sizeof(struct in_addr), AF_INET); //gets the host entry

    if (hostEntry == NULL) //if there is no host
    {
        fprintf(stderr, "No host found %d\n", errno);
        exit(1);
    }

    hostName = hostEntry->h_name;              //gets the hostName
    printf("Connection from: %s\n", hostName); //prints out who connected

    //sends current time and date to client
    char buffer[256];
    time_t curtime;
    curtime = time(NULL); //To get the date
    snprintf(buffer, sizeof(buffer), "%.24s\r\n", ctime(&curtime));
    write(connectfd, buffer, strlen(buffer));

    while (flag == 0)
    {
        size_t len = read(connectfd, &cmd, 256);
        cmd[len] = 0;
        printf("on read: %s\n", cmd);
        char choice[256];
        char rest[256];
        memset(choice, 0, 256);
        memset(rest, 0, 256);
        printf("|%s|%s|\n", choice, rest);
        split_space(cmd, choice, rest);
        printf("/%s/%s/\n", choice, rest);
        if (strncmp(choice, "exit", 3) == 0)
        {
            printf("receiving from client: %s\n", choice);
            printf("Quiting...\n");
            flag = 1;
        }

        else if (strncmp(choice, "cd", 2) == 0)
        {
            printf("receiving from client: %s\n", choice);
        }

        else if (strncmp(choice, "ls", 2) == 0)
        {
            printf("receiving from client: %s\n", choice);
        }

        else if (strncmp(choice, "get", 3) == 0)
        {
            char new_path[256];
            FILE *fp;
            char *buffer;
            long file_len;

            fp = fopen(rest, "rb");
            fseek(fp, 0, SEEK_END); // Jump to the end of the file
            file_len = ftell(fp);   // Get the current byte offset in the file
            rewind(fp);             // Jump back to the beginning of the file

            buffer = (char *)malloc((file_len + 1) * sizeof(char)); // Enough memory for file + \0
            fread(buffer, file_len, 1, fp);                         // Read in the entire file
            fclose(fp);                                             // Close the file

            write(connectfd, buffer, file_len);
            free(buffer);
        }

        else if (strncmp(choice, "show", 4) == 0)
        {
            printf("receiving from client: %s\n", choice);
        }

        else if (strncmp(choice, "put", 3) == 0)
        {
            printf("receiving from client: %s\n", choice);
        }

        else if (strncmp(choice, "rcd", 3) == 0)
        {
            char new_path[256];
            fun_cd(rest, &new_path);
            write(connectfd, new_path, 256);
        }

        else if (strncmp(choice, "rls", 3) == 0)
        {
            char result[256];
            fun_ls(result);
            write(connectfd, result, 256);
        }
    }

    return 0;
}
