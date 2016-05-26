#include <string.h>
#include "token.h"
#include "common.h"

// free the variables in a token object.
void Free(token *t)
{
    free(t->cmd);
    free(t->pos);
    return;
}

// generate a token object from a string command.
// do not use this function for a existing token.
void NewToken(token *t, str_t cmd)
{
    uint32_t i, j;
    uint32_t *posBuf;
    uint32_t len = strlen(cmd) + 1;

    t->cmd = malloc(len * sizeof(char));
    t->num = 1;

    posBuf = malloc((len / 2 + 1) * sizeof(uint32_t));
    posBuf[0] = 0;

    // find all the tokens
    // use the string to buffer the positions
    for (i = 0; i < len; i++) {
        t->cmd[i] = cmd[i];
        if (cmd[i] == ' ' || cmd[i] == 0) {
            posBuf[j] = i + 1;
            j++;
        }
    }

    memcpy(t->pos, posBuf, j * sizeof(uint32_t));
    t->num = j - 1;
    return;
}

// execute a token.
void Execute(token *t);
