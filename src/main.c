#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>
#include <readline/readline.h>
#include <readline/history.h>

int main()
{
    char* input, shell_prompt[100];

    while(1) {
        // Create prompt string from user name and current working directory.
        snprintf(shell_prompt, sizeof(shell_prompt), "%s:%s $ ", getenv("USER"), getcwd(NULL, 1024));

        // Display prompt and read input (n.b. input must be freed after use)...
        input = readline(shell_prompt);

        // Check for EOF.
        if (!input) {
            break;
        }

        // Add input to history.
        add_history(input);

        // exit instruction.
        if (!strcmp(input, "exit")) {
            break;
        }

        // execute on the input command.
        ExecuteLine(input);

        // Free input.
        free(input);
    }
    return 0;
}

bool Find(const char *dst, const char *dir)
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
