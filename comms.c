#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <string.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netinet/in.h>
#include <netdb.h> 
#include "comms.h"

#define PORT     15006
#define SERVER_ADDR "127.0.0.1"

static int sockfd;
static struct sockaddr_in servaddr;
static int serverlen;

int connectToPythonDisplayServer(char *pCaption, char *pCapStorage)
{
    struct hostent *server;
    char *hostname= "127.0.0.1";
    int port= 15006;
    // connect to python socket
    // Creating socket file descriptor
    if ( (sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0 )
    { 
        perror("socket creation failed");
	return(-1);
    }
    /* gethostbyname: get the server's DNS entry */
    server = gethostbyname(hostname);
    if(server == NULL)
    {
        fprintf(stderr,"ERROR, no such host as %s\n", hostname);
	return(-1);
    }
    memset(&servaddr, 0, sizeof(servaddr));
    // Filling server information 
    servaddr.sin_family = AF_INET; 
    servaddr.sin_port = htons(port);
    bcopy((char *)server->h_addr,
	  (char *)&servaddr.sin_addr.s_addr, server->h_length);
    serverlen = sizeof(servaddr);
    memset(pCapStorage, 0, 100);
    strncpy(pCapStorage, pCaption, 100);
    /* wait for the server to be ready and connect */
    printf("waiting for server\n");
    int ret= 0;
    while(1)
    {
        ret = connect(sockfd, (struct sockaddr *)&servaddr, serverlen);
	if(ret < 0)
	{
	    continue;
	}
	break;
    }
    printf("connected to server\n");
    return(0);
}

int update(int *pData, int numItems)
{
    int n;
    --pData;
    *pData = (int)htonl(COMMS_DISPLAY_UPDATE);
    n = send(sockfd, pData, (numItems + 1) * sizeof(int), 0 );
    if(n < 0)
    {
        printf("send failed - assuming python process aborted; ending\n");
	exit(0);
    }
    else if(n == 0)
    {
        printf("no data sent\n");
    }
    return(0);
}

int displayCaption(char *pName)
{
    int n;
    n = send(sockfd, pName, strlen(pName), 0 );
    if(n < 0)
    {
        perror("ERROR in send");
	return(-1);
    }
    else if(n == 0)
    {
        printf("no data sent\n");
    }
    return(0);
}

int getParameters(int *pNumItems, int *pItemMax)
{
    int n;
    char data[2 * 4 * sizeof(int)];
    while(1)
    {
        n = recvfrom(sockfd, (void *)data, 2 * 4 * sizeof(int), 0, (struct sockaddr *)&servaddr, &serverlen);
        if(n < 0)
        {
            perror("ERROR in recvFrom()");
	    continue;
        }
        else if(n == 0)
        {
            printf("no data received\n");
        }
	break;
    }
    // extract data
    *pNumItems = (int)htonl(*(unsigned int *)&(data[0]));
    *pItemMax = (int)htonl(*(unsigned int *)&(data[4]));
    return(0);
}
