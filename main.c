/**
 * @brief invokes a test-bed for algorithms; parses the command line; launches a python display
 *        server in a separate thread; searches the data section, algsxn, for the command-line
 *        specified algorithm; sends a message to the server to set the display server caption;
 *        sends a message to the display server requesting parameters; starts the algorithm;
 *        exits;
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

#define CAPTION_MAX_SZ 100

static int dataWithRoomForType[ARRAY_SZ];
static char caption[CAPTION_MAX_SZ];
int *data= &(dataWithRoomForType[1]);
int numItems;
int itemMax;

/**
 * @brief prints program usage
 *
 */
static void usage( void )
{
    printf( "usage:  ./alg -a <algorithm>\n" );
    printf( "  algorithms:\n" );
    struct sxnStruct *pStruct= &__start_algsxn;
    while( pStruct != &__stop_algsxn )
    {
	if( pStruct->name != NULL )
	{
	    printf( "    %s\n", pStruct->name );
	}
	pStruct++;
    }
    exit(1);
}

/**
 * @brief requests/processes user input
 *
 */
static int menu( void )
{
    printf( "menu:\n" );
    printf( "  1 - run again\n" );
    printf( "  2 - refresh\n" );
    printf( "  any other key - quit\n" );
    char inStr[20];
    fgets( inStr, 20, stdin );
    int retVal= 0;
    /* Leverage the break feature of do-while() to reduce
     * complexity of if-else and indentation
     */
    do
    {
	if( !strcmp(inStr,"1\n") )
	{
	    retVal = 1;
	    break;
	}
	if( !strcmp(inStr,"2\n") )
	{
	    retVal = 2;
	    update(data, numItems);
	    break;
	}
    } while( 0 );
    return retVal;
}

/**
 * @brief command line parser
 *
 * @param argc - number of command line arguments including the program name
 * @param argv[] - array of command line argument strings
 *
 * @return
 *    /li pointer to sxnStruct structure
 */
static struct sxnStruct *parseCmdLine( int argc, char *argv[] )
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
    }
    struct sxnStruct *pStruct= &__start_algsxn;
    while( pStruct != &__stop_algsxn )
    {
	if( !strcmp( pStruct->name, algorithm ) )
	{
	    printf( "found algoritm %s\n", algorithm );
	    break;
	}
	pStruct++;
    }
    if( pStruct == &__stop_algsxn )
    {
	printf( "could not find algoritm %s, quitting\n", algorithm );
	usage();
    }
    return pStruct;
}

/**
 * @brief invokes a test-bed for algorithms; parses the command line; launches a python display
 *        server in a separate thread; searches the data section, algsxn, for the command-line
 *        specified algorithm; sends a message to the server to set the display server caption;
 *        sends a message to the display server requesting parameters; starts the algorithm;
 *        exits;
 *
 * @param argc - number of command line arguments including the program name
 * @param argv[] - array of command line argument strings
 *
 */ 
void main(int argc, char *argv[])
{
    struct sxnStruct *pStruct= parseCmdLine( argc, argv );
    launchDisplay();
    memset(caption, 0, CAPTION_MAX_SZ);
    snprintf(caption, CAPTION_MAX_SZ, "%s display", pStruct->displayStr);
    displayCaption(caption);
    getParameters(&numItems, &itemMax);
    numItems = ntohl(numItems);
    itemMax = ntohl(itemMax);
    printf("main(): getParameters() returned numItems= %d, itemMax= %d\n",
	   numItems, itemMax);
    printf( "main() starting algorithm %s\n", pStruct->name );
    pStruct->algFxn();
    while(1)
    {
	int retVal= menu();
	if(retVal == 1)
	{
	    pStruct->algFxn();
	    continue;
	}
	if(retVal == 2)
	{
	    continue;
	}
	break;
    }
    terminateServer();
    joinThread();
    exit(0);
}
