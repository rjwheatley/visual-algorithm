/**
 * @brief bubble() - the bubblesort sorting algorithm
 *
 */
#include "alg.h"
#include "loadData.h"

extern int numItems;
extern int itemMax;
extern int *data;

/**
 * @brief bubble sort algorithm
 *
 */ 
static void bubble(void)
{
    /* initial data */
    _Bool conventional= 1;
    _Bool noDuplicates= 0;
    loadData( 0, NULL, conventional );
    update(data, numItems);
    int swap = 0;
    while(1)
    {
        swap = 0;
        int ndx;
        for(ndx = 1 ; ndx < numItems ; ++ndx )
        {
            if(data[ndx - 1] > data[ndx])
            {
                int temp = data[ndx - 1];
                data[ndx - 1] = data[ndx];
                data[ndx] = temp;
                update(data, numItems);
                swap = 1;
            }
        }
        if(swap == 0)
        {
            break;
        }
    }
    return;
}

static char algName[]= "bubble";
static char dispName[]= "bubble sort";
static struct sxnStruct sxnS __attribute__ ((section("algsxn"))) = { algName, dispName, bubble };
