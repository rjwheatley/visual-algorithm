#pragma once

#define ARRAY_SZ 2000
#define EXIT_STATUS 0x55

extern int init(void);
extern int connectToPythonDisplayServer(void);
extern int update(int *pData, int numItems);
extern int displayCaption(char *pName);
extern int getParameters(int *pNumItems, int *pItemMax);

enum
{
    COMMS_INVALID,
    COMMS_DISPLAY_UPDATE,
    COMMS_DISPLAY_CAPTION,
    COMMS_DISPLAY_GET_PARAMS,
    COMMS_TERMINATE
};
