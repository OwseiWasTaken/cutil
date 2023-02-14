#ifndef cutil
#define cutil

// Includes
#include <string.h>
#include <termios.h>
#include <time.h>
#include <signal.h>
#include <math.h>
#include <assert.h>
#include <fcntl.h>

#include <dirent.h>

#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/types.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <stdbool.h>

// Types
//typedef unsigned char uint8;
typedef uint8_t uint8;
//typedef signed char int8;
typedef int8_t int8;

typedef short int int16;
typedef uint8_t byte;
typedef long double lfloat;
// cresent values
typedef uint64_t kbkey;
//typedef unsigned long kbkey;

typedef struct {
	int year, day, hour, minute, seccond;
	// NULL terminated
	char weekday[4], month[4];
} fmttime;

typedef struct {
	int y, x;
} point;

typedef struct {
	uint32_t y, x;
} upoint;

typedef struct {
	lfloat a, r;
} polar;

typedef struct {
	lfloat y, x;
} ppoint;

typedef struct {
	byte R, G, B;
} color;

typedef struct {
	byte first; // first item in queue
	byte last; // last item in queue relative to first
	int qu[255]; // queue
} queue_t;

typedef bool safeguard;

//TODO: dynamic int
// cresent values
typedef struct {
	byte *values;
	size_t size;
} dint;

// Definitios
#define PI 3.141592

// max len for single key, +1 for \0
#define MAXSCHARLEN 8

// i guess
#define MAXCOLORLEN 20

// escape char
#define ESC "\x1b" // 27

// max (default) number of opened files
// soft limit
#define MAXFDS 1024
// hard limit = 1'048'576

// mmap flag for not anon (just for style, since backed is defautl)
#define MAP_BACKED 0

// tab (since \t is just several spaces)
#define TAB "  "

// default size for io buffer // only set after StopChTerm
static char stdinbuff[BUFSIZ];

// Globals
// last key buffer, when StartCh is on
char _lk[MAXSCHARLEN]; // new stdin buffer
// last key id
kbkey _keyid;
// unbuffered ch
bool _StartCh;

// funcs used before they are defined
int CountDecimalDigits( int v );


// Colors
#define	Red "\x1b[38;2;255;0;0m"
#define	White "\x1b[38;2;255;255;255m"
#define	nc "\x1b[38;2;255;255;255m"

// Cutil files
// keys
#include "keys.c"

// funcs
#include "stuff.c"

#endif
