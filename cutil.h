#ifndef cutil
#define cutil 1

// Inclues
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

// Types
typedef signed char int8;
typedef unsigned int uint;
typedef unsigned char uint8;
typedef unsigned char byte;
// cresent values
typedef unsigned long kbkey;

typedef struct {
	int year, day, hour, minute, seccond;
	char weekday[4], month[4]; // NULL terminated
} fmttime;

typedef struct {
	int y, x;
} point;

typedef struct {
	float y, x;
} ppoint;

typedef struct {
	byte R, G, B;
} color;

// cresent values
typedef struct {
	byte *values;
	size_t size;
} dint;

// Definitios
#define PI 3.141592
// max len for single key, +1 for \0
#define MAXSCHARLEN 9
// i guess
#define MAXCOLORLEN 19

// Globals
// last key buffer, when StartCh is on
char _lk[MAXSCHARLEN];
// last key id
kbkey _keyid;
// default size for io buffer
char stdinbuff[BUFSIZ];
// unbuffered ch
bool _StartCh;

// Colors
#define	Red "\x1b[38;2;255;0;0m"
#define	White "\x1b[38;2;255;255;255m"
#define	nc "\x1b[38;2;255;255;255m"

// Cutil files
// funcs
#include "keys.c"

// keys
#include "stuff.c"

#endif
