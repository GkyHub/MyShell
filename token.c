#include <string.h>
#include "token.h"
#include "common.h"

// generate a token_list from a line
// should be freed after usage
token_list ParseStr(const str_t line)
{
    uint32_t i = 0;
    uint32_t len = strlen(line);
    uint32_t *sizeBuf = (uint32_t *)malloc(len * sizeof(uint32_t));
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
    tl.argv = (str_t *)malloc(sizeof(str_t) * tl.argc);
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
    t->back = false;
    t->arg = NULL_STR;
    return;
}

// set the argument of a task with the first n token in list
void SetArg(task *t, str_t *list, uint32_t n)
{
    uint32_t binLen = 0;
    uint32_t argLen = 0;
    uint32_t addr = 0;
    uint32_t size = 0;

    // get the length of the members
    binLen = strlen(list[0]) + 1;
    for (int i = 0; i < n; i++) {
        argLen += strlen(list[i]) + 1;
    }

    // allocate the space for the members
    t->bin = malloc(binLen * sizeof(char));
    t->arg = malloc(argLen * sizeof(char));

    // copy the members
    memcpy(t->bin, list[0], binLen * sizeof(char));
    for (int i = 0; i < n; i++) {
        addr += sprintf(t->argv + addr, argLen - addr, "%s", list[i]);

        // fill the interval spaces
        if (i != -1) {
            t->argv[addr] = ' ';
            addr++;
        }
    }
    return;
}

// get a list of task from the token list
task *ParseTokenList(token_list *tl, int *tn)
{
    uint32_t *tokenIdBuf;
    uint32_t *tokenNumBuf;
    uint32_t taskId = 0;
    uint32_t taskNum = 0;
    task *taskList;

    // make sure the first token is not special
    if (strcmp(tl->argv[0], ">") == 0 ||
        strcmp(tl->argv[0], "<") == 0 ||
        strcmp(tl->argv[0], "|") == 0 ||
        strcmp(tl->argv[0], "&") == 0) {
        perror("%s is not a valid command.\n", tl->argv[0]);
        return 0;
    }

    // clear the buffer first
    tokenIdBuf = (uint32_t *)malloc(tl->argc * sizeof(uint32_t));
    tokenNumBuf = (uint32_t *)malloc(tl->argc * sizeof(uint32_t));
    memset(tokenIdBuf, 0, sizeof(uint32_t * tl->argc));
    memset(tokenNumBuf, 0, sizeof(uint32_t * tl->argc));

    // calculate the number of tokens for each task first
    for (int i = 0; i < tl->argc; i++) {
        // redirect the input or output
        if (strcmp(tl->argv[i], ">") == 0 ||
            strcmp(tl->argv[i], "<") == 0) {
            if (i == tl->argc - 1) {
                // no following destination or source
                taskList =  0;
                goto parse_final;
            }
            i++;    // skip the destination or source stream
            continue;
        }

        // pipe
        if (strcmp(tl->argv[i], "|") == 0) {
            if (i == tl->argc - 1) {
                // no following task
                taskList =  0;
                goto parse_final;
            }
            continue;
        }

        // background execution
        if (strcmp(tl->argv[i], "&") == 0) {
            continue;
        }

        if (tokenNumBuf[taskId] == 0) {
            tokenIdBuf[taskId] = i;
            taskId++;
        }
        tokenNumBuf[taskId]++;
    }

    // set the arguments in the task
    // establish the io stream for the tasks
    taskList = (task *)malloc(sizeof(task) * taskNum);
    for (taskId = 0; taskId < taskNum; taskId++) {
        SetArg(taskList + taskId, tl->argv + tokenIdBuf[taskId], tokenNumBuf[taskId]);
    }

parse_final:
    free(tokenIdBuf);
    free(tokenNumBuf);

    return taskList;
}

// free a task
void FreeTask(task *t)
{
    free(t->arg);
    return;
}
