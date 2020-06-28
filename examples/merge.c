/**
 * @brief merge() - the merge sort sorting algorithm
 *
 */
#include "alg.h"
#include "loadData.h"

extern int numItems;
extern int itemMax;
extern int *data;

/**
 * @brief Return the largest power-of-two not larger than n
 *
 */
static int powerOfTwo(int n)
{
    int power = 1;

    while(power < n)
    {
        power *= 2;
    }
    if(power != n)
    {
        power /= 2;
    }
    return(power);
}

/**
 * @brief Low level recursive routine which merges two already
 *        sorted lists
 *
 */
static void mrg(int ndx, int n, int loNdx, int hiNdx)
{
    int temp;
    int wrNdx= loNdx + hiNdx - (ndx + n / 2);

    if(loNdx >= ndx + (n / 2))
    {
        temp = data[hiNdx++];
    }
    else if(hiNdx >= ndx + n)
    {
        temp = data[loNdx++];
    }
    else
    {
        temp = (data[loNdx] < data[hiNdx]) ? data[loNdx++] : data[hiNdx++];
    }
    if(loNdx - ndx + hiNdx - (ndx + n / 2) < n)
    {
        mrg(ndx, n, loNdx, hiNdx);
    }
    data[wrNdx] = temp;
    update(data, numItems);
}

/**
 * @brief Top level recursive merge sort routine
 *
 */
static void mergeMerge(int ndx, int n)
{
    int n1, n2, ndx1, ndx2;

    if(n > 1)
    {
        if((n1 = powerOfTwo(n)) == n)
        {
            n2 = n1 = n/2;
        }
        else
        {
            n2 = n - n1;
        }
        ndx1 = ndx;
        ndx2 = ndx + n1;
        mergeMerge(ndx1, n1);
        mergeMerge(ndx2, n2);
        mrg(ndx, n, ndx1, ndx2);
    }
}

/**
 * @brief merge sort algorithm
 *
 */ 
static void merge(void)
{
    /* initial data */
    _Bool conventional= 1;
    _Bool noDuplicates= 0;
    loadData( 0, NULL, conventional );
    update(data, numItems);
    mergeMerge(0, numItems);

    return;
}

static char algName[]= "merge";
static char dispName[]= "merge sort";
static struct sxnStruct sxnS __attribute__ ((section("algsxn"))) = { algName, dispName, merge };
