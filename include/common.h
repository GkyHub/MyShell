#ifndef _COMMON_H_
#define _COMMON_H_

// data type definition.
typedef long long   int64_t;
typedef int         int32_t;
typedef short       int16_t;
typedef char        int8_t;

typedef unsigned long long  uint64_t;
typedef unsigned int        uint32_t;
typedef unsigned short      uint16_t;
typedef unsigned char       uint8_t;
typedef unsigned char       bool;

typedef char * str_t;

// constant definition.
#define NULL_STR 0;

// standard binary file location.
#define SYS_BIN     "/usr/bin/"
#define LOCAL_BIN   "/usr/local/bin/"
#define GAME_BIN    "/usr/games"

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
#define ERR(INFO) print(2, INFO)
// judge if an argument is a certain kind
#define ARG_IS(ARG, X) ((strcmp((ARG), (K))) == 0)

#endif
