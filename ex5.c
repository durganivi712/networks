#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <time.h>
#include <errno.h>

#define PORT 8080
#define BUFFER_SIZE 1024
#define DB_FILE "library.txt"
#define LOG_FILE "library_log.txt"

void getTime(char *t)
{
    time_t now = time(NULL);
    struct tm *tm_info = localtime(&now);

    strftime(t, 30, "%Y-%m-%d %H:%M:%S", tm_info);
}

void logActivity(char *message)
{
    FILE *fp;
    char t[30];

    getTime(t);

    fp = fopen(LOG_FILE, "a");

    if (fp != NULL)
    {
        fprintf(fp, "[%s] %s\n", t, message);
        fclose(fp);
    }
}

void viewBooks(char *response)
{
    FILE *fp;
    char line[200];

    response[0] = '\0';

    fp = fopen(DB_FILE, "r");

    if (fp == NULL)
    {
        strcpy(response, "Database file not found.");
        return;
    }

    while (fgets(line, sizeof(line), fp) != NULL)
    {
        strcat(response, line);
    }

    fclose(fp);
}

void searchBook(char *book, char *response)
{
    FILE *fp;
    char line[200];
    int found = 0;

    response[0] = '\0';

    fp = fopen(DB_FILE, "r");

    if (fp == NULL)
    {
        strcpy(response, "Database file not found.");
        return;
    }

    while (fgets(line, sizeof(line), fp) != NULL)
    {
        if (strstr(line, book) != NULL)
        {
            strcat(response, line);
            found = 1;
        }
    }

    fclose(fp);

    if (!found)
        strcpy(response, "Book not found.");
}

void updateBook(char *book, int issue)
{
    FILE *fp, *temp;
    char line[200];
    char name[100];
    int copies;
    int found = 0;

    fp = fopen(DB_FILE, "r");
    temp = fopen("temp.txt", "w");

    if (fp == NULL || temp == NULL)
        return;

    while (fgets(line, sizeof(line), fp) != NULL)
    {
        if (sscanf(line, "%[^|]|%d", name, &copies) == 2)
        {
            if (strcmp(name, book) == 0)
            {
                found = 1;

                if (issue == 1)
                {
                    if (copies > 0)
                        copies--;
                }
                else
                {
                    copies++;
                }

                fprintf(temp, "%s|%d\n", name, copies);
            }
            else
            {
                fputs(line, temp);
            }
        }
    }

    fclose(fp);
    fclose(temp);

    remove(DB_FILE);
    rename("temp.txt", DB_FILE);

    if (!found)
        logActivity("Invalid book operation.");
}

void issueBook(char *book, char *response)
{
    FILE *fp;
    char line[200], name[100];
    int copies, found = 0;

    fp = fopen(DB_FILE, "r");

    if (fp == NULL)
    {
        strcpy(response, "Database error.");
        return;
    }

    while (fgets(line, sizeof(line), fp) != NULL)
    {
        if (sscanf(line, "%[^|]|%d", name, &copies) == 2)
        {
            if (strcmp(name, book) == 0)
            {
                found = 1;

                if (copies > 0)
                {
                    updateBook(book, 1);
                    strcpy(response, "Book issued successfully.");
                    logActivity("Book issued.");
                }
                else
                {
                    strcpy(response, "Book is not available.");
                }
                break;
            }
        }
    }

    fclose(fp);

    if (!found)
        strcpy(response, "Book not found.");
}

void returnBook(char *book, char *response)
{
    FILE *fp;
    char line[200], name[100];
    int copies, found = 0;

    fp = fopen(DB_FILE, "r");

    if (fp == NULL)
    {
        strcpy(response, "Database error.");
        return;
    }

    while (fgets(line, sizeof(line), fp) != NULL)
    {
        if (sscanf(line, "%[^|]|%d", name, &copies) == 2)
        {
            if (strcmp(name, book) == 0)
            {
                found = 1;
                updateBook(book, 0);
                strcpy(response, "Book returned successfully.");
                logActivity("Book returned.");
                break;
            }
        }
    }

    fclose(fp);

    if (!found)
        strcpy(response, "Book not found.");
}

int main()
{
    int sockfd;
    struct sockaddr_in server, client;
    socklen_t len;
    char buffer[BUFFER_SIZE];
    char response[BUFFER_SIZE];
    char command[20], book[100];

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    if (sockfd < 0)
    {
        perror("Socket error");
        exit(1);
    }

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(PORT);

    if (bind(sockfd, (struct sockaddr *)&server, sizeof(server)) < 0)
    {
        perror("Bind error");
        exit(1);
    }

    printf("Library UDP Server started...\n");

    while (1)
    {
        len = sizeof(client);

        recvfrom(sockfd, buffer, sizeof(buffer) - 1, 0,
                 (struct sockaddr *)&client, &len);

        buffer[strcspn(buffer, "\n")] = '\0';

        printf("Request received: %s\n", buffer);

        /*
           Send ACK immediately.
           This provides application-level acknowledgement.
        */
        sendto(sockfd, "ACK", 3, 0,
               (struct sockaddr *)&client, len);

        sscanf(buffer, "%s %[^\n]", command, book);

        if (strcmp(command, "VIEW") == 0)
        {
            viewBooks(response);
        }
        else if (strcmp(command, "SEARCH") == 0)
        {
            searchBook(book, response);
        }
        else if (strcmp(command, "ISSUE") == 0)
        {
            issueBook(book, response);
        }
        else if (strcmp(command, "RETURN") == 0)
        {
            returnBook(book, response);
        }
        else
        {
            strcpy(response, "Invalid request.");
        }

        sendto(sockfd, response, strlen(response), 0,
               (struct sockaddr *)&client, len);

        logActivity(buffer);
    }

    close(sockfd);
    return 0;
}



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/time.h>

#define PORT 8080
#define BUFFER_SIZE 1024
#define MAX_RETRY 3

int main()
{
    int sockfd;
    struct sockaddr_in server;
    socklen_t len;
    char buffer[BUFFER_SIZE];
    char response[BUFFER_SIZE];

    int choice;
    int retry;
    struct timeval tv;

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    if (sockfd < 0)
    {
        perror("Socket error");
        exit(1);
    }

    /*
       Timeout = 2 seconds
    */
    tv.tv_sec = 2;
    tv.tv_usec = 0;

    setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO,
               &tv, sizeof(tv));

    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    server.sin_addr.s_addr = inet_addr("127.0.0.1");

    while (1)
    {
        printf("\n===== LIBRARY MANAGEMENT =====\n");
        printf("1. Search Book\n");
        printf("2. Issue Book\n");
        printf("3. Return Book\n");
        printf("4. View Books\n");
        printf("5. Exit\n");
        printf("Enter choice: ");
        scanf("%d", &choice);
        getchar();

        if (choice == 5)
            break;

        if (choice == 1)
        {
            char book[100];

            printf("Enter book name: ");
            fgets(book, sizeof(book), stdin);
            book[strcspn(book, "\n")] = '\0';

            sprintf(buffer, "SEARCH %s", book);
        }
        else if (choice == 2)
        {
            char book[100];

            printf("Enter book name: ");
            fgets(book, sizeof(book), stdin);
            book[strcspn(book, "\n")] = '\0';

            sprintf(buffer, "ISSUE %s", book);
        }
        else if (choice == 3)
        {
            char book[100];

            printf("Enter book name: ");
            fgets(book, sizeof(book), stdin);
            book[strcspn(book, "\n")] = '\0';

            sprintf(buffer, "RETURN %s", book);
        }
        else if (choice == 4)
        {
            strcpy(buffer, "VIEW");
        }
        else
        {
            printf("Invalid choice.\n");
            continue;
        }

        /*
           Retransmission until ACK is received
        */
        for (retry = 1; retry <= MAX_RETRY; retry++)
        {
            sendto(sockfd, buffer, strlen(buffer), 0,
                   (struct sockaddr *)&server, sizeof(server));

            printf("Request sent. Attempt %d\n", retry);

            len = sizeof(server);

            int n = recvfrom(sockfd, response,
                             sizeof(response) - 1, 0,
                             (struct sockaddr *)&server, &len);

            if (n > 0)
            {
                response[n] = '\0';

                if (strcmp(response, "ACK") == 0)
                {
                    printf("ACK received.\n");

                    n = recvfrom(sockfd, response,
                                 sizeof(response) - 1, 0,
                                 (struct sockaddr *)&server, &len);

                    if (n > 0)
                    {
                        response[n] = '\0';
                        printf("\nServer Response:\n%s\n", response);
                    }

                    break;
                }
            }
            else
            {
                printf("Timeout! Retransmitting...\n");
            }
        }

        if (retry > MAX_RETRY)
            printf("Server not responding.\n");
    }

    close(sockfd);
    return 0;
}