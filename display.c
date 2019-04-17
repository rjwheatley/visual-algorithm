/**
 * @brief code to launch a python thread which is a server receiving requests
 *        over a TCP socket and which displays data graphically using the
 *        Pygame module.
 *
 */
#include <stdio.h>
#include <python2.7/Python.h>
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
#include <pthread.h>
#include "comms.h"

#ifndef h_addr
#define h_addr h_addr_list[0] /* for backward compatibility */
#endif

#define PORT     15006
#define SERVER_ADDR "127.0.0.1"

static int sockfd;
static struct sockaddr_in servaddr;
static int serverlen;
static pthread_t theThread;

/**
 * @brief code to run in a thread and launch a python display window
 *
 * @param argPtr - to satisfy pthread_create
 *
 */ 
static void *theThreadCode(void *argPtr)
{
    FILE* file;
    int argc;
    char * argv[3];
    // start python display
    printf("starting python display\n");
    argc = 1;
    argv[0] = "display.py";
    Py_SetProgramName(argv[0]);
    Py_Initialize();
    PySys_SetArgv(argc, argv);
    file = fopen("display.py","r");
    PyRun_SimpleFile(file, "display.py");
    Py_Finalize();
}

/**
 * @brief called to launch python display window (server) and connect
 *        to it via tcp as a client
 *
 * @param pCaption - pointer to a caption string to display at the
 *                   top of the window
 * @param pCapStorage - a pointer to storage for the caption stting
 *
 * @return 0
 */ 
int launchDisplay(char *pCaption, char *pCapStorage)
{
    int portno;
    struct hostent *server;
    // start python display
    printf("starting python thread\n");
    if(pthread_create(&theThread, NULL, theThreadCode, NULL))
    {
	  fprintf(stderr, "Error creating thread\n");
	  return(-1);
    }
    printf("done starting python thread\n");
    connectToPythonDisplayServer(pCaption, pCapStorage);
    
    char *hostname= "127.0.0.1";
    int port= 15006;
    // connect to python socket
    // Creating socket file descriptor
    if ( (sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0 )
    { 
        perror("socket creation failed");
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
    return(0);
}

/**
 * @brief Terminate the python display thread
 *
 * @return 0
 */ 
int joinThread(void)
{
    if(pthread_join(theThread, NULL))
    {
        fprintf(stderr, "Error joining thread\n");
	return(-1);
    }
    return(0);
}
