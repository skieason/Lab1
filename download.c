#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#define SOCKET_ERROR        -1
#define BUFFER_SIZE         5000
#define HOST_NAME_SIZE      255
#define FILE_NAME_SIZE      255
#define MAXMSG              1024

int isNumber(char number[])
{
    int i = 0;

    //checking for negative numbers
    if (number[0] == '-')
        i = 1;
    for (; number[i] != 0; i++)
    {
        //if (number[i] > '9' || number[i] < '0')
        if (!isdigit(number[i]))
            return 0;
    }
    return 1;
}


int  main(int argc, char* argv[]) {
    int hSocket;                 /* handle to socket */
    struct hostent* pHostInfo;   /* holds info about a machine */
    struct sockaddr_in Address;  /* Internet socket address struct */
    long nHostAddress;
    char pBuffer[BUFFER_SIZE];
    unsigned nReadAmount;
    char strHostName[HOST_NAME_SIZE];
    char strPortName[HOST_NAME_SIZE];
    int nHostPort;
    char strFileName[FILE_NAME_SIZE];
    char option;
    int dOpt = 0, cOpt = 0, count = 0, success = 0;
    int originalCount;

    if(argc < 4) {
        perror("\nUsage: ./download host-name host-port URL\n");
        return 0;
    } else if (argc == 4){
        strcpy(strHostName,argv[1]);
        if (isNumber(argv[2]) == 0) {
            perror("\n Usage: ./download host-name host-port URL\n Port must be a number\n");
            return 0;
        }
        nHostPort=atoi(argv[2]);
        strcpy(strFileName,argv[3]);
    } else if (argc == 5) {
        option = getopt(argc, argv, "d");
        if (option == '?') {
            perror("\nUsage: ./download -d host-name host-port URL\n");
            return 0;
        }
        dOpt = 1;
        strcpy(strHostName,argv[2]);
        if (isNumber(argv[3]) == 0) {
            perror("\n Usage: ./download host-name host-port URL\n Port must be a number\n");
            return 0;
        }
        nHostPort=atoi(argv[3]);
        strcpy(strFileName,argv[4]);
    } else if (argc == 6) {
        option = getopt(argc, argv, "c");
        if (option == '?') {
            perror("\nUsage: ./download -c count host-name host-port URL\n");
            return 0;
        }
        cOpt = 1;
        count = atoi(argv[2]);
        originalCount = atoi(argv[2]);
        strcpy(strHostName,argv[3]);
        if (isNumber(argv[4]) == 0) {
            perror("\n Usage: ./download host-name host-port URL\n Port must be a number\n");
            return 0;
        }
        nHostPort=atoi(argv[4]);
        strcpy(strFileName,argv[5]);
    }

    do {
        /* make a socket */
        hSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

        if (hSocket == SOCKET_ERROR) {
            printf("\nCould not make a socket\n");
            return 0;
        }

        /* get IP address from name */
        pHostInfo = gethostbyname(strHostName);
        if (pHostInfo == NULL) {
            perror("\nHostname Invalid\n");
            return 0;
        }

        /* copy address into long */
        memcpy(&nHostAddress, pHostInfo->h_addr, pHostInfo->h_length);

        /* fill address struct */
        Address.sin_addr.s_addr = nHostAddress;
        Address.sin_port = htons(nHostPort);
        Address.sin_family = AF_INET;

//    printf("\nConnecting to %s (%X) on port %d",strHostName,nHostAddress,nHostPort);

        /* connect to host */
        if (connect(hSocket, (struct sockaddr *) &Address, sizeof(Address))
            == SOCKET_ERROR) {
            printf("\nCould not connect to host\n");
            return 0;
        }

        char *message = (char *) malloc(MAXMSG);
        if (dOpt == 1) {
            sprintf(message, "GET %s HTTP/1.1\r\nHost:%s:%d\r\n\r\n", strFileName, strHostName, nHostPort);
            printf("\n%s", message);
        } else if (cOpt == 1) {
            sprintf(message, "GET %s HTTP/1.1\r\nHost:%s:%d\r\n\r\n", strFileName, strHostName, nHostPort);
        } else {
            sprintf(message, "GET %s \r\nHTTP/1.1\r\nHost:%s:%d\r\n\r\n", strFileName, strHostName, nHostPort);
        }
        write(hSocket, message, strlen(message));
//    do {
        count--;
        memset(pBuffer, 0, BUFFER_SIZE);
        nReadAmount = read(hSocket, pBuffer, BUFFER_SIZE);

        if (strstr(pBuffer, "200 OK") != NULL) {
            success++;
        }
//    } while (count > 0);
//
//        if (cOpt == 1) {
//            printf("\n Successfully downloaded the file %d/%d times", success, originalCount);
//        } else {
//            printf("\n %s", pBuffer);
//        }

        if (close(hSocket) == SOCKET_ERROR) {
            printf("\nCould not close socket\n");
            return 0;
        }

        free(message);
    } while (count > 0);
    if (cOpt == 1) {
        printf("\n Successfully downloaded the file %d/%d times", success, originalCount);
    } else {
        printf("\n %s", pBuffer);
    }
}




//    } else {
//        memset(pBuffer, 0, BUFFER_SIZE);
//        nReadAmount=read(hSocket,pBuffer,BUFFER_SIZE);
//        printf("\n %s",pBuffer);
//    }
//    write(hSocket,message,strlen(message));


//    strstr(read(hSocket,pBuffer, BUFFER_SIZE - 1), "200 OK");

//    char *responseHeaders = (char*) malloc(MAXHEADERS);
//    int contentLength = 0;
//    memset(pBuffer, 0, BUFFER_SIZE);
//    while(read(hSocket, pBuffer, BUFFER_SIZE - 1) != 0) {
////        printf("%s", pBuffer);
//        strcat(responseHeaders, pBuffer);
//        if (strstr(pBuffer, "\r\n\r\n") == NULL) {
//
//        }
//        memset(pBuffer, 0, BUFFER_SIZE);
//    }
//    nReadAmount=read(hSocket,pBuffer,BUFFER_SIZE);
//    if (cOpt != 1) {
//        printf("\n %s",pBuffer);
//    } else {
//        printf("\n%d",success);
//    }
//    char *response = (char *) malloc(contentLength);
