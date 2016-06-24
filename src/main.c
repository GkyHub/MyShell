#include "token.h"
#include "common.h"
#include <ncurses.h>
#include <readline/readline.h>
#include <readline/history.h>


int main()
{
    char* input, shell_prompt[100];

    InitTaskList();

    printf("\n==========================\n");
    printf("YAUSH initialize finish.");
    printf("\n==========================\n");

    while(1) {
        // Create prompt string from user name and current working directory.
        snprintf(shell_prompt, sizeof(shell_prompt), "%s:%s $ ", getenv("USER"), getcwd(NULL, 1024));

        // Display prompt and read input (n.b. input must be freed after use)...
        input = readline(shell_prompt);

        // Check for EOF.
        if (!input) {
            break;
        }

        if (strlen(input) == 0) {
            continue;
        }

        // Add input to history.
        add_history(input);

        // exit instruction.
        if (strcmp(input, "exit") == 0) {
            break;
        }

        // execute on the input command.
        ExecuteLine(input);

        // Free input.
        free(input);
    }
    return 0;
}
