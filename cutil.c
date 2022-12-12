// IO
void StartChTerm ( FILE* log ) {
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

byte GetCh() {
	// clean lk
	memset(lk, 0, MAXSCHARLEN);
	// get single char, set lk
	return getc(stdin);
}

kbkey LtoK() {
	kbkey r = 0;
	int MaxI = strlen(lk);
	for (int i = 0; i<MaxI; i++) {
		r += (byte) lk[i]<<(8*i);
	}
	return r;
}

kbkey GetChId() {
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
fmttime FmtTime(time_t rn, int UTF) {
	fmttime now;

	char* timetext = ctime(&rn);

	//TODO: test this
	memcpy(now.weekday, (timetext), 3);
	//now.weekday[0] = timetext[0];
	//now.weekday[1] = timetext[1];
	//now.weekday[2] = timetext[2];
	now.weekday[4] = '\0';

	memcpy(now.month, (timetext+4), 3);
	//now.month[0] = timetext[4];
	//now.month[1] = timetext[5];
	//now.month[2] = timetext[6];
	now.month[3] = '\0';

	now.minute = (rn%(60*60))/60;
	now.seccond= rn%60;
	now.year = 1970+(int)(rn/(24.0*3600.0)/365.25);
	now.hour = rn/3600%24+(UTF);
	if (now.hour < 1) {
		now.hour = 24+now.hour;
	}

	now.day = timetext[9]-'0';
	if (timetext[8] != ' ') {
		now.day += 10*(timetext[8]-'0');
	};
	return now;
}

char* FmtTimeToString(fmttime now) {
	char* buff = malloc(38);
	sprintf(buff, "s:%d\nm:%d\nh:%d\nd:%d\ny:%d\n\nM:%s\nW:%s",
		now.seccond, now.minute, now.hour, now.day, now.year, now.month, now.weekday);
	return buff;
}

// Math
int ipow(int base, int power) {
	return (int)pow((double)base, (double)power);
}

int isquare (int base) {
	return ipow(base, 2);
}

void move(int y, int x) {
	printf("\x1B[%d;%dH", y+1, x+1);
}

void HideCursor() {
	puts("\x1b[?25l");
}

void ShowCursor() {
	puts("\x1b[?25h");
}

// ANSII (text) rgb
char* TRGB(uint8 R, uint8 G, uint8 B) {
	char* buff = malloc(21);
	sprintf(buff, "\x1b[38;2;%d;%d;%dm", R, G, B);
	return buff;
}

point GetTerminalSize() {
	struct winsize w;
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
	point p = {.y = w.ws_col, .x = w.ws_row};
	return p;
}

