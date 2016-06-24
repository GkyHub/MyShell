#ifndef _COMMON_H_
#define _COMMON_H_

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <dirent.h>
#include <fcntl.h>
#include <string.h>
#include <sys/wait.h>

// data type definition.
typedef unsigned long long  uint64_t;
typedef unsigned int        uint32_t;
typedef unsigned short      uint16_t;
typedef unsigned char       uint8_t;
typedef unsigned char       bool;

typedef char * str_t;

// constant definition.
#define NULL_STR 0
#define TRUE     1
#define FALSE    0

// standard binary file location.
#define BIN_FOLDER_NUM 5
extern const str_t g_binFolder[BIN_FOLDER_NUM];

// built in function name list
#define BUILT_IN_NUM 1
extern const str_t g_builtInList[BUILT_IN_NUM];
extern int (*g_builtInFunc[BUILT_IN_NUM])(str_t argv[]);

// special mark for command execution.
#define M_RED_STD_OUT ">"
#define M_RED_STD_ERR "2>"
#define M_RED_STD_IN  "<"
#define M_RUN_BG      "&"
#define M_PIPE        "|"
#define M_DIVIDE      ";"

// parser status machine.
#define PAR_ARG     1   // next is the argument
#define PAR_NEW     2   // a new command
#define PAR_RED_IN  3   // redirect input
#define PAR_RED_OUT 4   // redirect output
#define PAR_RED_ERR 5   // redirect error

// function definition.
// standard error output
#define ERR(INFO) fprintf(STDERR_FILENO, INFO)
// judge if an argument is a certain kind
#define ARG_IS(ARG, X) ((strcmp(ARG, X)) == 0)

#endif
