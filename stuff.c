// IO
void StartChTerm (FILE* log) {
	if (StartCh) return;
	setvbuf(stdin, lk, _IOFBF, MAXSCHARLEN);
	StartCh = true;

	struct termios term;
	tcgetattr(STDIN_FILENO, &term);
	if (log) fprintf(log, "this tty: %s\n", ttyname(STDIN_FILENO));

	system("/bin/stty -g > /tmp/restore");
	if (log) fprintf(log, "tty config saved to '/tmp/restore'\n");

	char reconfig[36+10]; /* 36 = comamnd len, 10 = /dev/ttyNN */
	sprintf(reconfig, "/bin/stty -F %s -echo cbreak min 1", ttyname(STDIN_FILENO));
	if (log) fprintf(log, "tty reconfigured so no system-side buffering\n");
	system(reconfig);
}

void StopChTerm (FILE* log) {
	if (!StartCh) return;
	setvbuf(stdin, stdinbuff, _IOLBF, BUFSIZ);
	StartCh = false;

	system("/bin/stty $(/bin/cat /tmp/restore)");
	if (log) fprintf(log, "\ntty config restored\n");

	system("/bin/rm /tmp/restore");
	if (log) fprintf(log, "file '/tmp/restore' deleted\n");
}

byte GetCh () {
	// clean lk
	memset(lk, 0, MAXSCHARLEN);
	// get single char, set lk
	return getc(stdin);
}

kbkey LtoK () {
	kbkey r = 0;
	int MaxI = strlen(lk);
	for (int i = 0; i<MaxI; i++) {
		r += (byte) lk[i]<<(8*i);
	}
	return r;
}

kbkey GetChId () {
	memset(lk, 0, MAXSCHARLEN);
	getc(stdin);
	kbkey r = 0;
	int MaxI = strlen(lk);
	for (int i = 0; i<MaxI; i++) {
		r += (byte) lk[i]<<(8*i);
	}
	return r;
}

// Time
fmttime FmtTime (const time_t rn, const int UTF) {
	fmttime now;

	char* timetext = ctime(&rn);

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

//TODO: not dynamic
void FmtTimeToString (const fmttime now, char* buff) {
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
	printf("\x1B[%d;%dH", y+1, x+1);
}

void HideCursor () {
	puts("\x1b[?25l");
}

void ShowCursor () {
	puts("\x1b[?25h");
}

void TsRGB (const char* buff, color RGB) {
	sprintf(buff, "\x1b[38;2;%d;%d;%dm", RGB.R, RGB.G, RGB.B);
}

void TRGB (const char* buff, byte R, byte G, byte B) {
	sprintf(buff, "\x1b[38;2;%hhu;%hhu;%hhum", R, G, B);
}

const point GetTerminalSize () {
	struct winsize w;
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
	point p = {.y = w.ws_col, .x = w.ws_row};
	return p;
}

void _Print_Point(const point p) {
	printf("{%i,%i}", p.y, p.x);
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


