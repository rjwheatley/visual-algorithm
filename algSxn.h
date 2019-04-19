#pragma once

struct sxnStruct
{
    char *name;
    char *displayStr;
    void (*algFxn)(void);
};

extern struct sxnStruct __start_algsxn;
extern struct sxnStruct __stop_algsxn;
