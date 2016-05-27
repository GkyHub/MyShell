#include "common.h"

// a token list parsed from the input command line
typedef struct token_list {
    uint32_t argc;
    str_t   *argv;
} token_list;

// a set of task will be generated from the token list
typedef struct task {
    int     io_stdin;
    int     io_stdout;
    int     io_stderr;
    str_t   arg;
} task;

// generate a token_list from a line
// should be freed after usage
token_list ParseStr(str_t line);

// free the list from token
void FreeTokenList(token_list *tl);

// initiate a task
void InitTask(task *t);

// get a list of task from the token list
task *ParseTokenList(token_list *tl, int &tn);

// free a task
void FreeTask(task *t);
