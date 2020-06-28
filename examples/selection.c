/**
 * @brief selection() - the selection sort sorting algorithm
 *
 */
#include "alg.h"
#include "loadData.h"

extern int numItems;
extern int itemMax;
extern int *data;

/**
 * @brief selection sort algorithm
 *
 */ 
static void selection(void)
{
	/* initial data */
    _Bool conventional= 1;
    _Bool noDuplicates= 0;
    loadData( 0, NULL, noDuplicates );
	update(data, numItems);
	for(int ondx = 0 ; ondx < numItems ; ++ondx )
	{
		int ndx;
		int smallestNdx = ondx;
		for(ndx = ondx + 1 ; ndx < numItems ; ++ndx )
		{
			if(data[ndx] < data[smallestNdx])
			{
				smallestNdx = ndx;
			}
		}
		int temp = data[ondx];
		data[ondx] = data[smallestNdx];
		data[smallestNdx] = temp;
		update(data, numItems);
	}
	return;
}

static char algName[]= "selection";
static char dispName[]= "selection sort";
static struct sxnStruct sxnS __attribute__ ((section("algsxn"))) = { algName, dispName, selection };
