#include "token.h"
#include "common.h"

task g_taskList[MAX_TASK_NUM];
const str_t g_binFolder[BIN_FOLDER_NUM] = {
    "/bin/",            // system binary
    "/usr/bin/",        // usr binary
    "/usr/local/bin/",  // local user binary
    "/usr/games/",      // games
    "./"                // current folder
};

const str_t g_builtInList[BUILT_IN_NUM] = {
    "cd"                // change directory
};
int (*g_builtInFunc[BUILT_IN_NUM])(str_t argv[]) = {
    CD                  // change directory
};

extern int32_t maxFid;

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
            sizeBuf[tokenId] = size + 1;
            tokenId++;
            size = 0;
        }
        else {
            size++;
        }
        i++;
    }
    sizeBuf[tokenId] = size + 1;

    // build the list
    tl.argc = tokenId + 1;
    tl.argv = (str_t *)malloc(sizeof(str_t) * tl.argc);
    for (i = 0; i < tl.argc; i++) {
        // malloc the space for a token
        tl.argv[i] = malloc(sizeBuf[i] * sizeof(char));
        // get the token
        memcpy(tl.argv[i], line + addr, sizeBuf[i] * sizeof(char));
        // move to the next token
        addr += sizeBuf[i];
        // set the end of the token
        tl.argv[i][sizeBuf[i] - 1] = 0;
    }

    free(sizeBuf);

    return tl;
}

// free the list from token
void FreeTokenList(token_list *tl)
{
    int i = 0;
    for (i = 0; i < tl->argc; i++) {
        free(tl->argv[i]);
    }
    free(tl->argv);
    return;
}

// get a list of task from the token list
uint32_t ParseTokenList(token_list *tl)
{
    uint32_t taskId = 0;
    uint32_t i;
    uint32_t parStat = PAR_NEW;    // parser status
    int32_t  fid[2];    // fid[0] for input; fid[1] for output; both for pipe

    // parse each of the token
    for (i = 0; i < tl->argc; i++) {
        switch (parStat) {
            case PAR_NEW: {
                g_taskList[taskId].argv[0] = tl->argv[i];
                g_taskList[taskId].argc++;
                parStat = PAR_ARG;
                break;
            }
            case PAR_ARG: {
                // redirect stdout
                if (ARG_IS(tl->argv[i], M_RED_STD_OUT)) {
                    parStat = PAR_RED_OUT;
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
                    if (taskId >= MAX_TASK_NUM - 1) {
                        fprintf(stderr, "Maximum task number in a command is %d. Exceeded.\n",
                            MAX_TASK_NUM);
                        ClearTaskList();
                        return -1;
                    }
                    pipe(fid);
                    g_taskList[taskId].io_stdout  = fid[1];
                    g_taskList[taskId+1].io_stdin = fid[0];
                    taskId++;
                    parStat = PAR_NEW;
                }
                // run in background.
                else if (ARG_IS(tl->argv[i], M_RUN_BG)) {
                    parStat = PAR_ARG;
                    g_taskList[taskId].back = 1;
                }
                // a new process.
                else if (ARG_IS(tl->argv[i], M_DIVIDE)) {
                    if (taskId >= MAX_TASK_NUM - 1) {
                        fprintf(stderr, "Maximum task number in a command is %d. Exceeded.\n",
                            MAX_TASK_NUM);
                        ClearTaskList();
                        return -1;
                    }
                    taskId++;
                    parStat = PAR_NEW;
                }
                // a parameter for the program itself.
                else {
                    if (g_taskList[taskId].argc >= MAX_PARAM_NUM - 1) {
                        fprintf(stderr, "Param number exceed maximum: %d", MAX_PARAM_NUM);
                        ClearTaskList();
                        return -1;
                    }
                    g_taskList[taskId].argv[g_taskList[taskId].argc] = tl->argv[i];
                    g_taskList[taskId].argc++;
                    parStat = PAR_ARG;  // remain the status
                }
                break;
            }
            case PAR_RED_IN: {
                if ((fid[0] = open(tl->argv[i], O_RDONLY)) < 0) {
                    fprintf(stderr, "Failed to open %s.\n", tl->argv[i]);
                    ClearTaskList();
                    return -1;
                }
                g_taskList[taskId].io_stdin = fid[0];
                parStat = PAR_ARG;
                break;
            }
            case PAR_RED_OUT: {
                if ((fid[1] = open(tl->argv[i], O_WRONLY | O_CREAT, 0666)) < 0) {
                    fprintf(stderr, "Failed to open %s.\n", tl->argv[i]);
                    ClearTaskList();
                    return -1;
                }
                g_taskList[taskId].io_stdout = fid[1];
                parStat = PAR_ARG;
                break;
            }
            case PAR_RED_ERR: {
                if ((fid[1] = open(tl->argv[i], O_WRONLY | O_CREAT)) < 0) {
                    fprintf(stderr, "Failed to open %s.\n", tl->argv[i]);
                    ClearTaskList();
                    return -1;
                }
                g_taskList[taskId].io_stderr = fid[1];
                parStat = PAR_ARG;
                break;
            }
            default: {
                fprintf(stderr, "Parser error at status %d!\n", parStat);
                ClearTaskList();
                exit(0);
            }
        }
    }

    // error processing
    switch(parStat) {
        case PAR_RED_IN: {
            fprintf(stderr, "No stdin redirection source.\n");
            ClearTaskList();
            return -1;
        }
        case PAR_RED_OUT: {
            fprintf(stderr, "No stdout redirection destination.\n");
            ClearTaskList();
            return -1;
        }
        case PAR_RED_ERR: {
            fprintf(stderr, "No stderr redirection destionation.\n");
            ClearTaskList();
            return -1;
        }
        default: {
            // no error
        }
    }
    return taskId + 1;
}

// free a task.
void ClearTaskList()
{
    int i = 0;
    int j = 0;
    for (i = 0; i < MAX_TASK_NUM; i++) {
        // wait for the task to finish if it is not running in background.
        if (g_taskList[i].pid > 0 && !g_taskList[i].back) {
            waitpid(g_taskList[i].pid, NULL, 0);
            g_taskList[i].pid = -1;
        }

        // clear the I/O.
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

        // set default to run in foreground.
        g_taskList[i].back = FALSE;

        // clear the pointers of the arguments.
        for (j = 1; j < MAX_PARAM_NUM; j++) {
            g_taskList[i].argv[j] = NULL;
        }
        g_taskList[i].argc = 0;
    }
    return;
}

void InitTaskList()
{
    int i = 0;
    int j = 0;
    for (i = 0; i < MAX_TASK_NUM; i++) {
        g_taskList[i].pid = -1;

        g_taskList[i].io_stdin = STDIN_FILENO;
        g_taskList[i].io_stdout = STDOUT_FILENO;
        g_taskList[i].io_stderr = STDERR_FILENO;

        g_taskList[i].back = FALSE;

        for (j = 1; j < MAX_PARAM_NUM; j++) {
            g_taskList[i].argv[j] = NULL;
        }

        g_taskList[i].argc = 0;
    }
    return;
}

// find a file under a directory. return if found or not.
bool Find(const str_t dst, const str_t dir)
{
    DIR *dp;
    struct dirent *entry;
    struct stat statbuf;

    // change to the directory
    if ((dp = opendir(dir)) == NULL) {
        fprintf(stderr, "cannot open directory: %s\n", dir);
        return FALSE;
    }

    while((entry = readdir(dp)) != NULL) {
        lstat(entry->d_name, &statbuf);
        // igore directory
        // a bug here, so we don't care directory.
        //if (S_ISDIR(statbuf.st_mode)) {
        //    printf("%s%s is directory\n", dir, entry->d_name);
        //    continue;
        //}
        if (strcmp(dst, entry->d_name) == 0) {
            return TRUE;
        }
    }

    return FALSE;
}

// Judge if a task is the shell built in one.
bool IsBuiltIn(task *pt)
{
    int i = 0;
    for (i = 0; i < BUILT_IN_NUM; i++) {
        if (strcmp(pt->argv[0], g_builtInList[i]) == 0) {
            return TRUE;
        }
    }
    return FALSE;
}

// Execute a built in task
int ExecBuiltIn(task *pt)
{
    int i = 0;
    for (i = 0; i < BUILT_IN_NUM; i++) {
        if (strcmp(pt->argv[0], g_builtInList[i]) == 0) {
            return g_builtInFunc[i](pt->argv);
        }
    }
    return -1;
}

// Main function for execution of a command line.
int ExecuteLine(const str_t cmdLine)
{
    token_list tl;
    uint32_t taskNum;
    uint32_t i, j, k;
    int32_t  pid;
    str_t    binPath;
    uint32_t binLen = 0, binPathLen;

    tl = ParseStr(cmdLine);
    taskNum = ParseTokenList(&tl);

    if (taskNum == 0) {
        // no task is generated, return
        return -1;
    }

    for (i = 0; i < taskNum; i++) {
        // execute built in function
        if (IsBuiltIn(&g_taskList[i])) {
            ExecBuiltIn(&g_taskList[i]);
            continue;
        }

        // execute external function
        if ((pid = fork()) == 0) {
            binLen = strlen(g_taskList[i].argv[0]);
            // child process, execute the task.
            // 1. find the binary. If not, return
            for (j = 0; j < BIN_FOLDER_NUM; j++) {
                if (Find(g_taskList[i].argv[0], g_binFolder[j])) {
                    binPathLen = strlen(g_binFolder[j]) + binLen;
                    binPath = (str_t)malloc((binPathLen + 1)*sizeof(char));
                    strcpy(binPath, g_binFolder[j]);
                    strcat(binPath, g_taskList[i].argv[0]);
                    break;
                }
            }

            if (j == BIN_FOLDER_NUM) {
                fprintf(stderr, "Command %s not found.\n", g_taskList[i].argv[0]);
                exit(0);
            }

            // 2. dup the I/O
            if (g_taskList[i].io_stdin != STDIN_FILENO) {
                dup2(g_taskList[i].io_stdin, STDIN_FILENO);
            }
            if (g_taskList[i].io_stdout != STDOUT_FILENO) {
                dup2(g_taskList[i].io_stdout, STDOUT_FILENO);
            }
            if (g_taskList[i].io_stderr != STDERR_FILENO) {
                dup2(g_taskList[i].io_stderr, STDERR_FILENO);
            }

            // close all the possible I/O
            // this is especially important for pipe
            for (k = 3; k < taskNum * 3 + 2; k++) {
                close(k);
            }

            // 3. execute the command.
            // printf("%d\n", g_taskList[i].argc);
            // printf("%s\n", g_taskList[i].argv[1]);
            if (execv(binPath, g_taskList[i].argv) < 0) {
                fprintf(stderr, "Exec %s failed!\n", binPath);
                exit(0);
            }

            // 4. exit.
            if (g_taskList[i].io_stdin != STDIN_FILENO) {
                close(STDIN_FILENO);
            }
            if (g_taskList[i].io_stdout != STDOUT_FILENO) {
                close(STDOUT_FILENO);
            }
            if (g_taskList[i].io_stderr != STDERR_FILENO) {
                close(STDERR_FILENO);
            }

            free(binPath);
            exit(0);
        }
        else {
            // parent process, continue
            g_taskList[i].pid = pid;
            continue;
        }
    }

    // wait for the tasks to finish
    ClearTaskList();

    // free the token list
    FreeTokenList(&tl);

    return 0;
}
