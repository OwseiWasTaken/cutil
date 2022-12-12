#ifndef cutil 
#define cutil

#include <termios.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <math.h>
#include <time.h>
#include <assert.h>
#include <fcntl.h>

#include <sys/types.h>
#include <stdbool.h>

typedef unsigned int  uint;
typedef unsigned char uint8;
typedef unsigned char byte;
typedef unsigned long kbkey;
typedef signed   char int8;

#define PI 3.141592
// Max len for single key, +1 for \0
#define MAXSCHARLEN 9
// last key buffer, when StartCh is on
char lk[MAXSCHARLEN];
// default size for io buffer
char stdinbuff[BUFSIZ];
bool StartCh;

typedef struct {
	int year, day, hour, minute, seccond;
	char weekday[4], month[4];
} fmttime;

typedef struct {
	int y, x;
} point;

// "dinamy" lib
#include "keys.c"

#include "cutil.c"

#endif
