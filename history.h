#ifndef _HISTORY_H_
#define _HISTORY_H_
#include "common.h"

typedef struct history{
    str_t    *cmd;     // command history as a circular buffer
    uint32_t *cmdLen;  // length of each command
    uint32_t  size;    // maximum number of commands in history
    uint32_t  curIdx;  // location of the position to be written
    uint32_t  rdPos;   // read index of the commands, relative to current position
} history;

// initiate a history block with the the given maximum size
void HistoryInit(history *h, uint32_t size);

// clear all the histroies.
void HistoryClear(history *h);

// write a command into history.
// overwrite the last one if necessary
void WriteHistory(history *h, const str_t cmd);

// move the read pointer.
void RdPtrUp(history *h);
void RdPtrDown(history *h);

// read a command from history at the current read pointer.
char *ReadHistory(history *h);


#endif
