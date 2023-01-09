#ifndef cutil_funcs
#define cutil_funcs 1

// IO
void StartChTerm (FILE *log) {
	if (_StartCh) return;
	setvbuf(stdin, _lk, _IOFBF, MAXSCHARLEN);
	_StartCh = true;

	struct termios term;
	tcgetattr(STDIN_FILENO, &term);
	if (log) fprintf(log, "this tty: %s\n", ttyname(STDIN_FILENO));

	system("/bin/stty -g > /tmp/restore");
	if (log) fprintf(log, "tty config saved to '/tmp/restore'\n");

	char reconfig[36+10]; /* 36 = comamnd len, 10 = /dev/ttyNN */
	// -F for this tty
	// -echo to show chars
	// cbreak to keep ^C -> SIGINT
	// min 1 = feed buffer to program
	sprintf(reconfig, "/bin/stty -F %s -echo cbreak min 1", ttyname(STDIN_FILENO));
	system(reconfig);
	if (log) fprintf(log, "tty reconfigured, so linefeed buffering is off\n");
}

void StopChTerm (FILE *log) {
	if (!_StartCh) return;
	setvbuf(stdin, stdinbuff, _IOLBF, BUFSIZ);
	_StartCh = false;

	system("/bin/stty $(/bin/cat /tmp/restore)");
	if (log) fprintf(log, "\ntty config restored\n");

	system("/bin/rm /tmp/restore");
	if (log) fprintf(log, "file '/tmp/restore' deleted\n");
}

inline bool IsChTermOn() { return _StartCh; }

byte GetCh () {
	// clean _lk
	memset(_lk, 0, MAXSCHARLEN);
	// get first char, set lk
	return getc(stdin);
}

// transform last key to key id, return id
kbkey LtoK () {
	kbkey r = 0;
	for (int i = 0; i<MAXSCHARLEN-1; i++) {
		r += (byte) _lk[i]<<(8*i);
	}
	_keyid = r;
	return r;
}

// check last key against check id
bool CheckKey(kbkey check) {
	return LtoK()==check;
}

// recomemded Ch funcs
// GetCh, LtoK, return key id
kbkey GetChId () {
	memset(_lk, 0, MAXSCHARLEN);
	getc(stdin);
	return LtoK();
}

// check last id against check id
bool CheckLastKey(kbkey check) {
	return _keyid==check;
}

// flush stdout
int flush() {return fflush(stdout);}

int GetFdCount () {
	int fdcount;
	// /self/ or getpid()
	DIR *dir = opendir("/proc/self/fd/");
	for (fdcount = 0; readdir(dir) != NULL; fdcount++);

	// print dir names
	//struct dirent *dp;
	//for (fdcount = 0; (dp = readdir(dir)) != NULL; fdcount++) {
	//	printf("%s\n", dp->d_name);
	//}

	closedir(dir);
	return fdcount - 3; // ., .., this dirfd
}

bool CheckValidFd(int fd) {
	int fdcount;
	char fdstr[CountDecimalDigits(fd)];
	sprintf(fdstr, "%i", fd);
	// /self/ or getpid()
	DIR *dir = opendir("/proc/self/fd/");

	// print dir names
	struct dirent *dp;
	for (fdcount = 0; (dp = readdir(dir)) != NULL; fdcount++) {
		if (!strcmp(dp->d_name, fdstr)) {
			closedir(dir);
			return false;
		}
	}

	closedir(dir);
	return true;
}

//TODO: get fd array, compare to array
//1 read, instead of N+1 reads
// (Max) N Compares instead of FDs*N
int GetValidFd() {
	int i;
	for (i = 0; !CheckValidFd(i); i++);
	if (i > MAXFDS) { return -1; } // TODO: set errno
	return i;
}

// Time
fmttime FmtTime (const time_t rn, const int UTF) {
	fmttime now;

	char *timetext = ctime(&rn);

	// string manipulation
	memcpy(now.weekday, (timetext), 3);
	now.weekday[4] = '\0';

	memcpy(now.month, (timetext+4), 3);
	now.month[3] = '\0';

	now.day = timetext[9]-'0';
	if (timetext[8] != ' ') {
		now.day += 10*(timetext[8]-'0');
	};

	// ctime manipulation
	now.minute = (rn%(60*60))/60;
	now.seccond= rn%60;
	now.year = 1970+(int)(rn/(24.0*3600.0)/365.25);
	now.hour = rn/3600%24+(UTF);
	if (now.hour < 1) {
		now.hour = 24+now.hour;
	}

	return now;
}

void FmtTimeToString (const fmttime now, char *buff) {
	sprintf(buff, "s:%d\nm:%d\nh:%d\nd:%d\ny:%d\n\nM:%s\nW:%s",
		now.seccond, now.minute, now.hour, now.day, now.year, now.month, now.weekday);
}

// Math
int ipow (const int base, const int power) {
	return (int)pow((double)base, (double)power);
}

int isquare (const int base) {
	return ipow(base, 2);
}

int isqrt(const int num) {
	return (int)sqrt((float)num);
}

int CountDecimalDigits( int v ) {
	int i;
	for (i = 0; v; i++) {v = v/10;}
	return i;
}

ppoint bhask (size_t a, size_t b, const size_t c) {
	ppoint p = {.x=0.0, .y=0.0};
	float delt = sqrt(isquare(b) - (4*a*c));
	b *= -1;
	a *= 2;
	p.x = (b + delt) / a;
	p.y = (b - delt) / a;
	return p;
}

// ANSII
void move (const int y, const int x) {
	printf(ESC"[%d;%dH", y, x);
}

void pmove (const point p) {
	printf(ESC"[%d;%dH", p.y, p.x);
}

inline void HideCursor () {
	puts(ESC"[?25l");
}

inline void ShowCursor () {
	puts(ESC"[?25h");
}

void ClearLine(const int y, const int x) {
	move(y, x);
	puts(ESC"[2K");
}

void pClearLine(const point p) {
	pmove(p);
	puts(ESC"[2K");
}

void TsRGB (char *buff, const color RGB) {
	sprintf(buff, ESC"[38;2;%d;%d;%dm", RGB.R, RGB.G, RGB.B);
}

void TRGB (char *buff, const byte R, const byte G, const byte B) {
	sprintf(buff, ESC"[38;2;%hhu;%hhu;%hhum", R, G, B);
}

void PRGB (const byte R, const byte G, const byte B) {
	printf(ESC"[38;2;%hhu;%hhu;%hhum", R, G, B);
}

upoint GetTerminalSize () {
	struct winsize w;
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
	upoint p = {.x = (uint)w.ws_col, .y = (uint)w.ws_row};
	return p;
}

inline char* BoolToStr(bool b) {
	return b?"true":"false";
}

// structs
void _Print_Point(const point p) {
	printf("{%i,%i}", p.y, p.x);
}

color RGB(byte r, byte g, byte b) {
	color c = {.R=r, .G=g, .B=b};
	return c;
}

int fsleep (long double t) {
	return usleep((int) lroundl(1000000.0 * t));
}

queue_t *MakeQueue() {
	queue_t *q = malloc(sizeof(queue_t));
	q->first = 0;
	q->last = 0;
	memset(q->qu, 0, 255*sizeof(int));
	return q;
}

queue_t *InitQueue(queue_t *q) {
	q->first = 0;
	q->last = 0;
	memset(q->qu, 0, 255*sizeof(int));
	return q;
}

void ReallocQueue(queue_t *q) {
	// queue: [?, ?, ?, 1, 2, 3, ?, ?]
	// points:          f  -  l
	// queue: [1, 2, 3, ?, ?]
	// points: f  -  l

	// memmov if last > first
	//int t[255] = {0};
	//memmove(t, q->qu+q->first, q->last*sizeof(int));
	//memmove(q->qu, t, 255);

	memmove(q->qu, q->qu+q->first, q->last*sizeof(int));
	q->last -= q->first;
	q->first = 0;
}

// 0 = ok
// -1 = realloc but failed
int QueueAppend(queue_t *q, int item) {
	if (q->qu[q->last]) {
		ReallocQueue(q);
	}
	if (q->qu[q->last]) {
		return -1;
	}
	q->qu[q->last] = item;
	q->last++;
	return 0;
}

int QueuePop(queue_t *q) {
	int ret = q->qu[q->first];
	//q->first=q->first+1;
	q->first++;
	return ret;
}

int QueueRead(queue_t *q) {
	return q->qu[q->first];
}

int QueueSize(queue_t *q) {
	return q->last;
}

int QueueOffset(queue_t *q) {
	return q->first;
}

void _P_queue(queue_t *q) {
	printf("%d->%d\n", q->first, q->last);
	printf("queue {\n"TAB);
	for (byte i = 0; i<16; i++) {
		for (byte j = 0; j<16; j++) {
			//printf("<%d:%d>  ", 16*i+j, q->qu[16*i+j]);
			if (i*8+j == q->first) {
				PRGB(255,128,0);
				printf("%d", q->qu[i*16+j]);
				PRGB(255,255,255);
			} else if (i*8+j == q->last-1) {
				PRGB(255,127,128);
				printf("%d", q->qu[i*16+j]);
				PRGB(255,255,255);
			} else {
				printf("%d", q->qu[i*16+j]);
			}
			putc(',', stdout);
			putc(' ', stdout);
		}
		printf("\b\b \n"TAB); // remove ", " put " \n\t"
	}
	puts("\b\b}");
}

int randint(int min, int max) {
	return (rand() % (max-min+1))+min;
}

bool odds(int onein) {
	return !randint(0, onein);
}

//TODO: dynamic int
//dint MakeDint(size_t size) {
//	dint d = {.size = size, .values = malloc(size)};
//	return d;
//}
//
//inline bool overflow(char c, int add) {
//	return ((c+add)%255)<c;
//}
//
//void _Print_Dint(const dint d) {
//	for (int i = 0; i<d.size; i++) {
//		printf("%hhu,", d.values[i]);
//	}
//	printf("\b \n");
//}
//
//void AddsiDint (dint *d, int add, int offset) {
//	byte c;
//	for (int i = 0; i<sizeof(add); i++) {
//		c = (byte)(add>>(8*i))%254;
//		printf("%i: add: %hhu from %u of: %d == %d\n",
//			i, c, add>>(8*i),
//			(d->values[i]+c)%255<d->values[i],
//			overflow(d->values[i], c)
//		);
//		if (!c) {continue;}
//		if (overflow(d->values[i], c)) {
//			d->values[i+1]++;
//		}
//		d->values[i] += c;
//	}
//}
//
//void FreeDint(dint *d) {
//	free(d->values);
//	d = NULL;
//}

#endif
