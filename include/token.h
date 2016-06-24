#ifndef _TOKEN_H_
#define _TOKEN_H_

#include "common.h"
#define MAX_TASK_NUM 8     // maximum number of task in one command
#define MAX_PARAM_NUM 32

// a token list parsed from the input command line.
typedef struct token_list {
    uint32_t argc;
    str_t   *argv;
} token_list;

// a set of task will be generated from the token list.
typedef struct task {
    int32_t io_stdin;
    int32_t io_stdout;
    int32_t io_stderr;
    bool    back;
    str_t   argv[32];   // no more than 32 parameters
    int32_t argc;
    int32_t pid;
} task;

// maximum file descriptor used
extern int32_t maxFid;

// static allocate the task list for a command
// this simplifies the construction of the tasks
extern task g_taskList[MAX_TASK_NUM];

// generate a token_list from a line.
// should be freed after usage.
token_list ParseStr(str_t line);

// free the list from token.
void FreeTokenList(token_list *tl);

// get a list of task from the token list.
uint32_t ParseTokenList(token_list *tl);

void InitTaskList();

// free a the task list
void ClearTaskList();

// recursively find a file under a directory. return if found.
bool Find(const str_t dst, const str_t dir);

// Main function for execution of a command line.
int ExecuteLine(const str_t cmdLine);

// Judge if a function is a built-in function
bool IsBuiltIn(task *pt);

// Do a built in Execution
int ExecBuiltIn(task *pt);

// change directory
int CD(str_t argv[]);

#endif
