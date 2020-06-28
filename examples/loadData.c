/*
 * loadData.c
 *
 *  Created on: Jun 12, 2020
 *      Author: coach
 */
#include "alg.h"
#include "loadData.h"

extern int numItems;
extern int itemMax;
extern int *data;

/**
 * @brief random data loader
 *
 */
void loadData( int ndx, struct dLink *pDLnk, _Bool conventional )
{
    if( conventional != 0 )
    {
        for(int ndx = 0; ndx < numItems ; ++ndx)
        {
            data[ndx] = rand() % itemMax + 1;
        }
    }
    else
    {
        static int count= 0;
        float floatFactor = (float)itemMax / (float)numItems;
        if( ndx >= numItems )
        {
            return;
        }
        float floatDatum = (float)ndx * floatFactor;
        int datum = (int)floatDatum;
        datum = (floatDatum - (float)datum) >= 0.5 ? datum + 1 : datum;
        struct dLink dl;
        int rNdx;
        struct dLink *pDL;

        while(1)
        {
            rNdx = rand() % numItems;
            pDL= pDLnk;
            while(pDL != NULL)
            {
                if(pDL->randNdx == rNdx)
                {
                    break;
                }
                pDL = pDL->pDLink;
            }
            if(pDL == NULL)
            {
                break;
            }
        }
        data[rNdx] = datum;
        dl.pDLink = pDLnk;
        dl.randNdx = rNdx;
        loadData(ndx + 1, &dl, 0);
    }
}
