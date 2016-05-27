#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>

int main()
{

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
