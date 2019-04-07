/**
 * @brief main() for C-code test-bed for algorithms; includes bogo-sort as example algorithm; displays
 *        data visually as the algorithm proceeds by sending it to a separate thread via TCP messages;
 *        server thread is coded in python and uses the Pygame module.
 *
 */
#include <stdio.h>
#include <python2.7/Python.h>
#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <string.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netinet/in.h>
#include <netdb.h> 
#include <pthread.h>
#include <ctype.h>
#include "display.h"
#include "comms.h"
#include "algSxn.h"

static int dataWithRoomForType[ARRAY_SZ];
static char caption[100];
int *data= &(dataWithRoomForType[1]);
int numItems;
int itemMax;

extern struct sxnStruct __start_algsxn;
extern struct sxnStruct __stop_algsxn;

void usage( void )
{
    printf( "usage:  sort -a <algorithm>\n" );
}

void main(int argc, char *argv[])
{
    char *algorithm= NULL;
    int option= 0;
    while( (option = getopt( argc, argv, "a:" )) != -1 )
    {
	switch( option )
	{
        case 'a':

            algorithm = optarg;
            break;
	}
    }
    if( algorithm == NULL )
    {
 	usage();
	exit(1);
    }
    launchDisplay("bogo sort display",caption);
    displayCaption(caption);
    getParameters(&numItems, &itemMax);
    numItems = ntohl(numItems);
    itemMax = ntohl(itemMax);
    printf("main(): getParameters() returned numItems= %d, itemMax= %d\n",
	   numItems, itemMax);
    printf("main() creating initial data\n");
    for(int ndx = 0; ndx < numItems ; ++ndx)
    {
      data[ndx] = htonl(rand() % itemMax + 1);
    }
    printf("main() sending data\n");
    update(data, numItems);
    printf("main() starting bogo sort\n");
    struct sxnStruct *pStruct= &__start_algsxn;
    while( pStruct != &__stop_algsxn )
    {
	if( !strcmp( pStruct->name, algorithm ) )
	{
	    pStruct->algFxn();
	}
	pStruct++;
    }
    joinThread();
    exit(0);
}
