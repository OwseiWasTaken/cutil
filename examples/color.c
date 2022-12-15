#include "cutil.h"

char strcol[MAXCOLORLEN];

int main(void) {
	StartChTerm(NULL);
	char TestStr[30];
	byte col[3] = {255,255,255};
	short x = 0;
	long k = 0;

	memcpy(TestStr ,"Lorem Ipsum", 12);

	while (k!=KEY_enter) {
		TRGB(strcol, col[0], col[1], col[2]);
		move(1, 1);
		printf("%3hhu,%3hhu,%3hhu\n", col[0], col[1], col[2]);
		printf("%s%s%s", strcol, TestStr, nc);
		move(1, 3+(4*x));
		flush();
		k = GetChId();
		switch (k) {
			case KEY_up:
				col[x]++;
				break;
			case KEY_down:
				col[x]--;
				break;
			case KEY_left:
				if (x) x--;
				break;
			case KEY_right:
				x=(x+1)%3;
				break;
		}
	}
	ClearLine(1,1);
	move(1,1);
	//TODO read argv and format accordingly
	printf("\\x1b[38;2;%hhu;%hhu;%hhum", col[0],col[1],col[2]);
	StopChTerm(NULL);
}

