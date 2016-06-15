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
uint32_t ParseTokenList(token_list *tl)
{
    uint32_t taskId = 0;
    uint32_t i;
    uint32_t parStat = PAR_NEW;    // parser status
    uint32_t binLen, argLen;
    int32_t  fid[2];    // fid[0] for input; fid[1] for output; both for pipe

    // parse each of the token
    for (i = 0; i < tl->argc; i++) {
        switch (parStat) {
            case PAR_NEW: {
                binLen = strlen(tl->argv[i]);
                g_taskList[taskId].bin = (str_t)malloc(sizeof(char) * (binLen + 1));
                strcpy(g_taskList[taskId].bin, tl->argv[i]);
                parStat = PAR_ARG;
                argLen = 0;
                break;
            }
            case PAR_ARG: {
                // redirect stdout
                if (ARG_IS(tl->argv[i], M_RED_STD_OUT)) {
                    parStat = PAR_RED_OUT
                }
                // redirect stdin
                else if (ARG_IS(tl->argv[i], M_RED_STD_IN)) {
                    parStat = PAR_RED_IN;
                }
                // redirect std error
                else if (ARG_IS(tl->argv[i], M_RED_STD_ERR)) {
                    parStat = PAR_RED_ERR;
                }
                // establish a pipe
                else if (ARG_IS(tl->argv[i], M_PIPE)) {
                    if (taskNum >= MAX_TASK_NUM) {
                        ERR("Maximum task number in a command is %d. Exceeded.\n",
                            MAX_TASK_NUM);
                        ClearTaskList();
                        return -1;
                    }
                    pipe(fid);
                    g_taskList[i].io_stdout = fid[1];
                    g_taskList[i].io_stdin  = fid[0];
                    parStat = PAR_NEW;
                }
                // run in background.
                else if (ARG_IS(tl->argv[i], M_RUN_BG)) {
                    parStat = PAR_ARG;
                    g_taskList[taskId].back = 1;
                }
                // a new process
                else if (ARG_IS(tl->argv[i], M_DIVIDE)) {
                    parStat = PAR_NEW;
                }
                else {
                    argLen += strlen(tl->argv[i]);
                    parStat = PAR_ARG;  // remain the status
                    break;
                }
            }
            case PAR_RED_IN: {
                fid[0] = open(argv[i], O_RDONLY);
                g_taskList[taskId].io_stdin = fid[0];
                parStat = PAR_ARG;
                break;
            }
            case PAR_RED_OUT: {
                fid[1] = open(argv[i], O_WRONLY);
                g_taskList[taskId].io_stdout = fid[1];
                parStat = PAR_ARG;
                break;
            }
            case PAR_RED_ERR: {
                ofId = open(argv[i], O_WRONLY);
                g_taskList[taskId].io_stderr = fid[1];
                parStat = PAR_ARG;
                break;
            }
            default: {
                ERR("Parser error at status %d!\n", parStat);
                ClearTaskList();
                exit(0);
            }
        }
    }

    // error processing
    switch(parStat) {
        case PAR_RED_IN: {
            ERR("No stdin redirection source.\n");
            ClearTaskList();
            return -1;
        }
        case PAR_RED_OUT: {
            ERR("No stdout redirection destination.\n");
            ClearTaskList();
            return -1;
        }
        case PAR_RED_ERR: {
            ERR("No stderr redirection destionation.\n");
            ClearTaskList();
            return -1;
        }
        default: {
            // no error
        }
    }
    return taskNum;
}

// free a task.
void ClearTaskList(uint32_t tn)
{
    int i = 0;
    for (i = 0; i < tn; i++) {
        // wait for the task to finish if it is not running in background
        if (g_taskList[i].pid > 0 && !g_taskList.back) {
            waitpid(g_taskList[i].pid);
            g_taskList[i].pid = -1;
        }

        // clear the I/O
        if (g_taskList[i].io_stdin != STDIN_FILENO) {
            close(g_taskList[i].io_stdin);
            g_taskList[i].io_stdin = STDIN_FILENO;
        }
        if (g_taskList[i].io_stdout != STDOUT_FILENO) {
            close(g_taskList[i].io_stdout);
            g_taskList[i].io_stdout = STDOUT_FILENO;
        }
        if (g_taskList[i].io_stderr != STDERR_FILENO) {
            close(g_taskList[i].io_stderr);
            g_taskList[i].io_stderr = STDERR_FILENO;
        }

        // set default to run in foreground
        g_taskList[i].back = false;

        // clear the string space for bin and arg
        if (g_taskList[i].bin != NULL_STR) {
            free(g_taskList[i].bin);
            g_taskList[i].bin = NULL_STR;
        }
        if (g_taskList[i].arg != NULL_STR) {
            free(g_taskList[i].arg);
            g_taskList[i].arg = NULL_STR;
        }
    }
    return;
}

// find a file under a directory. return if found or not.
bool Find(const str_t dst, const str_t dir)
{
    DIR *dp;
    struct dirent *entry;

    // change to the directory
    if ((dp = opendir(dir)) == NULL) {
        printf("cannot open directory: %s\n", dir);
        return false;
    }

    while((entry = readdir(dp)) != NULL) {
        lstat(entry->d_name, &statbuf);
        // igore directory
        if (S_ISDIR(statbuf.st_mode)) {
            continue;
        }
        if (strcmp(dst, entry->d_name) == 0) {
            return true;
        }
    }
    return false;
}

// Main function for execution of a command line.
int ExecuteLine(const str_t cmdLine)
{
    token_list tl;
    uint32_t taskNum;
    uint32_t i;
    int32_t  pid;

    tl = ParseStr(cmdLine);
    pTasks = ParseTokenList(tl, taskNum);
    FreeTokenList(tl);

    if (taskNum == 0) {
        // no task is generated, return
        return -1;
    }

    for (i = 0; i < taskNum; i++) {
        if ((pid = fork()) == 0) {
            // child process, execute the task.
            // change the I/O file descriptor.
            if (Find(g_taskList[i].bin, SYS_BIN)) {

            }
            else if (Find(g_taskList[i].bin, LOCAL_BIN)) {

            }
            else if (Find(g_taskList[i].bin, GAME_BIN)) {

            }
            // execute the command.

            // exit.
        }
        else {
            // parent process, continue
            continue;
        }
    }

    // wait for the tasks to finish
    ClearTaskList();

    return 0;
}
