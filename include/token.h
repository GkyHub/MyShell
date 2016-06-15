#include "common.h"
#define MAX_TASK_NUM 8;     // maximum number of task in one command

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
    str_t   bin;
    str_t   arg;
    int32_t pid;
} task;

// static allocate the task list for a command
// this simplifies the construction of the tasks
static task g_taskList[MAX_TASK_NUM];

// generate a token_list from a line.
// should be freed after usage.
token_list ParseStr(str_t line);

// free the list from token.
void FreeTokenList(token_list *tl);

// initiate a task.
void InitTask(task *t);

// set the argument of a task with the first n token in list.
void SetArg(task *t, str_t *list, uint32_t n);

// get a list of task from the token list.
task *ParseTokenList(token_list *tl, int &tn);

// free a the task list
void ClearTaskList();

// recursively find a file under a directory. return if found.
bool Find(const str_t dst, const str_t dir);

// Main function for execution of a command line.
int ExecuteLine(const str_t cmdLine);
