/**
 * @brief insertion() - the insertion sort sorting algorithm
 *
 */
#include "alg.h"
#include "loadData.h"

extern int numItems;
extern int itemMax;
extern int *data;

/**
 * @brief insertion sort algorithm
 *
 */ 
static void insertion(void)
{
    /* initial data */
    _Bool conventional= 1;
    _Bool noDuplicates= 0;
    loadData( 0, NULL, conventional );
    update(data, numItems);
    for(int ondx = 1 ; ondx < numItems ; ++ondx )
    {
        int ndx;
        for(ndx = ondx ; ndx >= 0 ; --ndx )
        {
            if(data[ndx - 1] > data[ndx])
            {
                int temp = data[ndx - 1];
                data[ndx - 1] = data[ndx];
                data[ndx] = temp;
                update(data, numItems);
            }
            else
            {
                break;
            }
        }
    }
    return;
}

static char algName[]= "insertion";
static char dispName[]= "insertion sort";
static struct sxnStruct sxnS __attribute__ ((section("algsxn"))) = { algName, dispName, insertion };
