#include "cutil.h"

int main(int argc, char *argv[]) {
	StartChTerm(NULL); // getch, no logging
	bool TextProvided = false;
	char* TestStr;

	if ( argc == 2 ) {
		TextProvided = true;
		TestStr = argv[1];
	} else if ( argc > 2 ) {
		printf("usage:\n%s [Example string]\n", argv[0]);
		return 1;
	}

	if (!TextProvided) {
		TestStr = malloc(14);
		memcpy(TestStr ,"Lorem Ipsum\0", 13);
	}
	byte col[3] = {255,255,255};

	short x = 0;
	long k = 0;


	while (k!=KEY_enter) {
		move(1, 1);
		PRGB(255,255,255);
		printf("%3hhu,%3hhu,%3hhu\n", col[0], col[1], col[2]);
		PRGB(col[0], col[1], col[2]);
		printf("%s\n"nc, TestStr);
		move(1, 3+(4*x));
		flush();
		k = GetChId();
		switch (k) {
			case KEY_k:
			case KEY_w:
			case KEY_up:
				col[x]++;
				break;
			case KEY_j:
			case KEY_s:
			case KEY_down:
				col[x]--;
				break;
			case KEY_h:
			case KEY_a:
			case KEY_left:
				if (x) { x--; }
				else { x=2; }
				break;
			case KEY_l:
			case KEY_d:
			case KEY_right:
				x=(x+1)%3;
				break;
		}
	}
	if (!TextProvided) { free(TestStr); }
	ClearLine(1,1);
	move(1,1);
	//TODO read argv and format accordingly
	printf("\n\\x1b[38;2;%hhu;%hhu;%hhum", col[0],col[1],col[2]);
	StopChTerm(NULL);
}

