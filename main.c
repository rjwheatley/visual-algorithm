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
#include "display.h"
#include "comms.h"

static int dataWithRoomForType[ARRAY_SZ];
static int *data= &(dataWithRoomForType[1]);
static char caption[100];
int numItems;
int itemMax;

/**
 * @brief bogo sort algorithm
 *
 */ 
void bogo(void)
{
    while(1)
    {
	int ndx;
	for(ndx = 1 ; ndx < numItems ; ++ndx )
        {
	    if(data[ndx - 1] > data[ndx + 1])
	    {
	        break;
	    }
        }
	if(ndx < numItems)
	{
	    for(int ndx = 0; ndx < numItems ; ++ndx)
	    {
	      data[ndx] = htonl(rand() % itemMax + 1);
	    }
	    update(data, numItems);
	}
	else
	{
	    break;
	}
    }
    return;
}

void main()
{
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
    bogo();
    joinThread();
    exit(0);
}
