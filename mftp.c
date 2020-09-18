#include "mftp.h"

#define MY_PORT_NUMBER 49999

int fun_ls();
int fun_cd();

int main(char argc, char *argv[])
{
    int socketfd;
    socketfd = socket(AF_INET, SOCK_STREAM, 0); //creates the socket (domain, type, protocol)

    struct sockaddr_in servAddr;
    struct hostent *hostEntry;
    struct in_addr **pptr;

    memset(&servAddr, 0, sizeof(servAddr));

    //needed to make the connection
    servAddr.sin_family = AF_INET;
    servAddr.sin_port = htons(MY_PORT_NUMBER);

    if (argv[1] == NULL) //checks if there is a location
    {
        printf("input a location from where you are connecting\n");
        exit(1);
    }

    hostEntry = gethostbyname(argv[1]); //connects to the server

    pptr = (struct in_addr **)hostEntry->h_addr_list; //writes hostEntry to the h_addr_list

    memcpy(&servAddr.sin_addr, *pptr, sizeof(struct in_addr)); //copy in_addr bytes to servAdd.sin_addr

    int con;
    con = connect(socketfd, (struct sockaddr *)&servAddr, sizeof(servAddr)); //connection

    if (con < 0) //checks for connection error
    {
        fprintf(stderr, "connection error %d\n", errno);
        exit(1);
    }

    int valread;
    char buffer[256] = {0};

    valread = read(socketfd, buffer, 256);         //reads what was sent from server
    printf("receiving from server: %s\n", buffer); //prints what was received

    int flag = 0;

    while (flag == 0) //stays connected to server
    {
        //int cpid = fork();
        char choice[256];
        memset(choice, 0, 256); //set choice[256] to zeros
        char rest[256];
        memset(rest, 0, 256); //set rest[256] to zeros
        char *buff = NULL;
        size_t size = 256;

        printf("MFTP> ");
        getline(&buff, &size, stdin);

        //printf("|%s|\n", buff);

        //reads until first space
        int buff_len = strlen(buff);
        int posn = split_space(buff, choice, rest);
        //printf("%d\n", posn);
        printf("%s\n", choice);

        if (strncmp(choice, "cd", posn - 1) == 0) //stop reading at posn position
        {
            if (!rest[0]) //if only cd than it continues
            {
                // no options provided
                continue;
            }
            //printf("%s\n", choice);
            printf("%s\n", rest);
            write(socketfd, buff, strlen(buff));
            char result[256];
            fun_cd(rest, result); //calls the cd function
            printf("%s\n", result);
        }

        else if (strncmp(choice, "ls", 2) == 0) //checks for the first two
        {
            write(socketfd, choice, strlen(choice));
            fun_ls(); //calls the ls function
        }

        else if (strncmp(choice, "rls", 3) == 0)
        {
            write(socketfd, choice, strlen(choice));
        }

        else if (strncmp(choice, "get", 3) == 0)
        {
            write(socketfd, buff, strlen(buff));
            char result[256];
            size_t file_len = read(socketfd, result, 256); //reads what was sent from server
            if (file_len)
            {
                printf("server: downloaded %d bytes\n", file_len);
                FILE *fp;
                char *filename = basename(rest);
                fp = fopen(filename, "wb");
                fwrite(buffer, 1, sizeof(buffer), fp);
                fclose(fp);
            }
        }

        else if (strncmp(choice, "show", 4) == 0)
        {
            write(socketfd, choice, strlen(choice));
        }

        else if (strncmp(choice, "put", 3) == 0)
        {
            write(socketfd, choice, strlen(choice));
        }

        else if (strncmp(choice, "rcd", 3) == 0)
        {
            write(socketfd, buff, strlen(buff));
            char result[256];
            valread = read(socketfd, result, 256); //reads what was sent from server
            if (valread)
            {
                printf("server: %s\n", result);
            }
        }

        else if (strncmp(choice, "exit", 4) == 0) //if user type "exit" it quit
        {
            write(socketfd, choice, strlen(choice));
            flag = 1;
        }

        else
        {
            printf("command not supported\n");
        }
    }

    printf("exiting...\n");

    return 0;
}