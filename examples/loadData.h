struct dLink
{
	struct dLink *pDLink;
	int randNdx;
};

extern void loadData( int ndx, struct dLink *pDLnk, _Bool conventional );
