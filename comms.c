/**
 * @brief code that communicates, via TCP socket, with a python graphical display thread (see display.c)
 *
 */
#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <string.h>
#include <strings.h>
#include <sys/types.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netinet/in.h>
#include <netdb.h>
#include "comms.h"
#include "display.h"

#ifndef h_addr
#define h_addr h_addr_list[0] /* for backward compatibility */
#endif

#define PORT     15006
#define SERVER_ADDR "127.0.0.1"

static int sockfd;
static struct sockaddr_in servaddr;
static int serverlen;

/**
 * @brief waits for and connects to the python dsisplay server
 *        via tcp as a client
 *
 * @param pCaption - pointer to a caption string to display at the
 *                   top of the window
 * @param pCapStorage - a pointer to storage for the caption stting
 *
 * @return
 *    /li 0 - success
 *    /li -1 - failure
 */
int connectToPythonDisplayServer(void)
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

/* @brief perform a check on the return value of send()
 *        or recvfrom()
 *
 * @param status - what was returned by send() or recvFrom()
 * @param snd - check send() status if COMMS_STAT_CHK_SND,
 *        recvFrom() status otherwise
 *
 * @return
 *    /li 0 - on success
 *    /li -1 - on failure
 */
static int checkSndOrRcvStatus(int status, T_COMMS_STAT_CHK_E statChkType )
{
    int retVal = 0;
    /* Leverage the break feature of do-while() to reduce
     * complexity of if-else and indentation
     */
    do
    {
        if(status < 0)
        {
            if( statChkType == COMMS_STAT_CHK_SND )
            {
                printf("send failed - assuming python process aborted; ending\n");
                exit(0);
            }
            else
            {
                perror("ERROR in recvFrom()");
                retVal = -1;
                break;
            }
        }
        if(status == 0)
        {
            printf("no data %s\n", (statChkType == COMMS_STAT_CHK_SND) ? "sent" : "received");
            retVal = -1;
        }
    } while( 0 );
    return( retVal );
}

/* @brief Convert the integers in the specified array to a network
 *        format
 *
 * @param pData - pointer to the data to convert
 * @param numItems - amount of data
 *
 */
static void convertForNetwork(int *pData, int numItems)
{
    for( int ndx = 0 ; ndx < numItems ; ++ndx )
    {
        pData[ndx] =  htonl(pData[ndx]);
    }
}

/* @brief Convert the integers in the specified array from a network
 *        format
 *
 * @param pData - pointer to the data to convert
 * @param numItems - amount of data
 *
 */
static void convertFromNetwork(int *pData, int numItems)
{
    for( int ndx = 0 ; ndx < numItems ; ++ndx )
    {
        pData[ndx] =  ntohl(pData[ndx]);
    }
}

/* @brief Send a comms display update message, with the update data, to
 *        to the pygame display server.  Wait for the acknowlegement.
 *
 * @param pData - pointer to the data to send
 * @param numItems - amount of data
 *
 * @return
 *    /li 0
 */
int update(int *pData, int numItems)
{
    int n;
    char data[2 * sizeof(int)];
    --pData;
    *pData = (int)COMMS_DISPLAY_UPDATE;
    convertForNetwork( pData, numItems + 1 );
    /* Leverage the break feature of do-while() to reduce
     * complexity of if-else and indentation
     */
    do
    {
        n = send(sockfd, pData, (numItems + 1) * sizeof(int), 0 );
        int status = checkSndOrRcvStatus( n, COMMS_STAT_CHK_SND );
        if( status )
        {
            break;
        }
        n = recvfrom(sockfd, (void *)data, 2 * sizeof(int), 0, (struct sockaddr *)&servaddr, &serverlen);
        status = checkSndOrRcvStatus( n, COMMS_STAT_CHK_RCV );
        if( status )
        {
            break;
        }
        int type = (int)*((unsigned int *)&(data[0]));
        if( type != COMMS_DISPLAY_UPDATE )
        {
            printf("wrong type, %d, received\n", type );
            break;
        }
        status = (int)*(unsigned int *)&(data[4]);
        if( status != 0 )
        {
            if( status == EXIT_STATUS )
            {
                printf("\nupdate:  Exit status, 0x%x, received, exiting\n", status );
                joinThread();
                exit(0);
            }
            printf("bad status, %d, received\n", status );
            break;
        }
    } while( 0 );
    convertFromNetwork( pData, numItems + 1 );
    return(0);
}

/* @brief Send a comms display caption label message, with the caption string, to
 *        to the pygame display server.  Wait for the acknowlegement.
 *
 * @param pName - pointer to the string to send
 *
 * @return
 *    /li 0
 */
int displayCaption(char *pName)
{
    int n;
    char rcvData[2 * sizeof(int)];
    int sndData[100];
    sndData[0] = (int)htonl(COMMS_DISPLAY_CAPTION);
    strcpy((char *)&(sndData[1]), pName);
    /* Leverage the break feature of do-while() to reduce
     * complexity of if-else and indentation
     */
    do
    {
        n = send(sockfd, sndData, 4 + strlen(pName), 0 );
        int status = checkSndOrRcvStatus( n, COMMS_STAT_CHK_SND );
        if( status )
        {
            break;
        }
        n = recvfrom(sockfd, (void *)rcvData, 2 * sizeof(int), 0, (struct sockaddr *)&servaddr, &serverlen);
        status = checkSndOrRcvStatus( n, COMMS_STAT_CHK_RCV );
        if( status )
        {
            break;
        }
        int type = (int)*((unsigned int *)&(rcvData[0]));
        if( type != COMMS_DISPLAY_CAPTION )
        {
            printf("wrong type, %d, received\n", type );
            break;
        }
        status = (int)ntohl(*((unsigned int *)&(rcvData[4])));
        if( status != 0 )
        {
            printf("bad status, %d, received\n", status );
            break;
        }
    } while( 0 );
    return(0);
}

/* @brief Send a comms get parameters message to the pygame
 *        display server.  Wait for the acknowlegement
 *        containing the parameters
 *
 * @param pNumItems - pointer to where to write the number
 *                    of display items
 * @param pItemMax - pointer to where to write the maximun
 *                   value of an item
 *
 * @return
 *    /li 0
 */
int getParameters(int *pNumItems, int *pItemMax)
{
    int n;
    int data[4];
    data[0] = (int)htonl(COMMS_DISPLAY_GET_PARAMS);
    /* Leverage the break feature of do-while() to reduce
     * complexity of if-else and indentation
     */
    do
    {
        n = send(sockfd, data, sizeof(int), 0 );
        int status = checkSndOrRcvStatus( n, COMMS_STAT_CHK_SND );
        if( status )
        {
            break;
        }
        n = recvfrom(sockfd, (void *)data, 4 * sizeof(int), 0, (struct sockaddr *)&servaddr, &serverlen);
        status = checkSndOrRcvStatus( n, COMMS_STAT_CHK_RCV );
        if( status )
        {
            break;
        }
        int type = (int)*(unsigned int *)&(data[0]);
        if( type != COMMS_DISPLAY_GET_PARAMS )
        {
            printf("wrong type, %d, received\n", type );
            break;
        }
        status = (int)htonl(*(unsigned int *)&(data[1]));
        if( status != 0 )
        {
            printf("bad status, %d, received\n", status );
            break;
        }
        *pNumItems = (int)htonl(*(unsigned int *)&(data[2]));
        *pItemMax = (int)htonl(*(unsigned int *)&(data[3]));
    } while( 0 );
    return(0);
}

/* @brief Send a comms terminate message to the pygame display
 *        server.
 *
 * @return
 *    /li 0
 */
int terminateServer(void)
{
    int n;
    int data[1];
    data[0] = (int)htonl(COMMS_TERMINATE);
    n = send(sockfd, data, sizeof(int), 0 );
    checkSndOrRcvStatus( n, COMMS_STAT_CHK_SND );
    return(0);
}
