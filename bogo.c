/**
 * @brief bogo() - the bogosort sorting algorithm - creates a random array of integers, checks to see if it's
 *                 sorted; if so, done; if not, replaces the array with a new random array, checks again etc.
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
#include "algSxn.h"

extern int numItems;
extern int itemMax;
extern int *data;

/**
 * @brief bogo sort algorithm
 *
 */ 
static void bogo(void)
{
    /* initial data */
    for(int ndx = 0; ndx < numItems ; ++ndx)
    {
	data[ndx] = htonl(rand() % itemMax + 1);
    }
    update(data, numItems);
    while(1)
    {
        /* check if data is sorted */
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

static char algName[]= "bogo";
static char dispName[]= "bogo sort";
static struct sxnStruct sxnS __attribute((__section__("algsxn"))) = { algName, dispName, bogo };
