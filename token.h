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
    bool    back;
    str_t   bin;
    str_t   arg;
} task;

// generate a token_list from a line
// should be freed after usage
token_list ParseStr(str_t line);

// free the list from token
void FreeTokenList(token_list *tl);

// initiate a task
void InitTask(task *t);

// set the argument of a task with the first n token in list
void SetArg(task *t, str_t *list, uint32_t n);

// get a list of task from the token list
task *ParseTokenList(token_list *tl, int &tn);

// free a task
void FreeTask(task *t);
