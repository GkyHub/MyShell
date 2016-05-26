#include <string.h>
#include "history.h"
#include "common.h"

// initiate a history block with the the given maximum size
void HistoryInit(history *h, uint32_t size)
{
    if (size == 0) {
        printf("invalid histroy size!\n");
        exit(0);
    }
    h->cmd    = malloc(size * sizeof(str_t));
    h->cmdLen = malloc(size * sizeof(uint32_t));

    // 0 length means invalid command.
    memset(h->cmdLen, 0, size * sizeof(uint32_t));

    h->size   = size;
    h->curIdx = 0;
    h->rdIdx  = 0;
    return;
}

// clear all the histroies.
void HistoryClear(history *h)
{
    uint32_t i = (h->curIdx == 0) ? (size - 1) : (curIdx - 1);
    while(h->cmdLen[i] > 0) {
        free(h->cmd[i]);
        h->cmdLen[i] = 0;
        i = (i == 0) ? (size - 1) : (i - 1);
    }
    return;
}

// write a command into history.
// overwrite the last one if necessary
void WriteHistory(history *h, const char *cmd)
{
    // free the postion if there is a valid command
    if (h->cmdLen[h->curIdx] > 0) {
        free(h->cmd[curIdx]);
        h->cmd[curIdx] = 0;
    }
    // malloc a new space for the command
    len = strlen(cmd) + 1;
    h->cmd[h->curIdx] = malloc(len);
    h->cmdLen[h->curIdx] = len;
    memcpy(h->cmd[h->curIdx], cmd, len);

    // update the current position
    h->curIdx = (h->curIdx >= (size - 1)) ? 0 : h->curIdx + 1;

    // move the read pointer to current position
    h->rdIdx = 0;
    return;
}

// move the read pointer.
void RdPtrUp(history *h)
{
    uint32_t rdIdx;
    // move 1 pos up
    h->rdPos++;
    rdIdx = (h->curIdx >= h->rdPos) ? (h->curIdx - h->rdPos) :
        (h->curIdx + h->size - h->rdPos);

    // judge if the position is valid
    if (h->cmdLen[rdIdx] == 0 || h->rdPos >= h->size) {
        h->rdPos--;
    }

    return;
}

void RdPtrDown(history *h)
{
    uint32_t rdIdx;
    if (rdPos == 0) {
        return;
    }
    rdPos--;
    return;
}

// read a command from history at the current read pointer.
char *ReadHistory(history *h)
{
    if (rdPos == 0) {
        return NULL_STR;
    }

    uint32_t rdIdx = (h->curIdx >= h->rdPos) ? (h->curIdx - h->rdPos) :
        (h->curIdx + h->size - h->rdPos);

    str_t cmd = malloc(h->cmdLen[rdIdx]);
    memcpy(cmd, h->cmd[rdIdx], h->cmdLen[rdIdx]);
    return cmd;
}
