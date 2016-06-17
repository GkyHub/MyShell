#include "common.h"
#include "token.h"

int CD(str_t argv[])
{
    char dir[256];
    // change to the home
    if (argv[1] == NULL) {
        snprintf(dir, sizeof(dir), "/home/%s/", getenv("USER"));
        return chdir(dir);
    }
    // change to the given directory
    else {
        return chdir(argv[1]);
    }
}
