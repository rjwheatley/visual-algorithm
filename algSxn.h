#pragma once

struct __attribute__ ((packed)) sxnStruct
{
    char *name;
    char *displayStr;
    void (*algFxn)(void);
    char pad[8];
};

extern struct sxnStruct __start_algsxn;
extern struct sxnStruct __stop_algsxn;
