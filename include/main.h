#ifndef INCLUDE_MAIN_H
#define INCLUDE_MAIN_H

#define _GNU_SOURCE
extern char *program_invocation_name;
extern char *program_invocation_short_name;
#define VERSION "0.0.1"
#define TAG "alpha"

#define CRED  "\x1b[31m"
#define CYELL "\x1b[33m"
#define CRSET "\x1b[0m"

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

//#pragma message __FILE__
#endif // INCLUDE_MAIN_H