#include "common.h"

typedef struct token {
    str_t     cmd;  // string command
    uint32_t *pos;  // position of first char of each unit
    uint32_t  num;  // number of unit in the token
} token;

// free the variables in a token object.
void Free(token *t);

// generate a token object from a string command.
void NewToken(token *t, str_t cmd);

// execute a token.
void Execute(token *t);
