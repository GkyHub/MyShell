#include <string.h>
#include "token.h"
#include "common.h"

// generate a token_list from a line
// should be freed after usage
token_list ParseStr(const str_t line)
{
    uint32_t i = 0;
    uint32_t len = strlen(line);
    uint32_t *sizeBuf = malloc(len * sizeof(uint32_t));
    uint32_t size = 0;
    uint32_t tokenId = 0;
    uint32_t addr = 0;

    token_list tl;

    // count the number and size of tokens
    while(line[i]) {
        if (line[i] == ' ') {
            size++;
            sizeBuf[tokenId] = size;
            tokenId++;
            size = 0
        }
        else {
            size++;
        }
    }
    size++;
    sizeBuf[tokenId] = size;
    tokenId++;

    // build the list
    tl.argc = tokenId;
    tl.argv = malloc(sizeof(str_t) * tl.argc);
    for (i = 0; i < tl->argc; i++) {
        // malloc the space for a token
        tl.argc[i] = malloc(sizeBuf[i] * sizeof(char));
        // get the token
        memcpy(tl.argc[i], line + addr, sizeBuf[i] * sizeof(char));
        // move to the next token
        addr += sizeBuf[i];
        // set the end of the token
        tl.argc[i][sizeBuf[i] - 1] = 0;
    }

    free(sizeBuf);

    return tl;
}

// free the list from token
void FreeTokenList(token_list *tl)
{
    for (int i = 0; i < tl->argc; i++) {
        free(argv[i]);
    }
    return;
}

// Initiate a task
void InitTask(task *t)
{
    t->io_stdin = STDIN_FILENO;
    t->io_stdout = STDOUT_FILENO;
    t->io_stderr = STDERR_FILENO;
    arg = NULL_STR;
    return;
}

// get a list of task from the token list
task *ParseTokenList(token_list *tl, int &tn)
{
    for (int i = 0; i < tl->argc; i++) {
        // redirect the output
        if (strcmp(tl->argv[i], ">")) {

        }
    }
}

// free a task
void FreeTask(task *t)
{
    free(t->arg);
    return;
}
