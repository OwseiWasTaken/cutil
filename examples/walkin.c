#include "cutil.h"

int main(void) {
	StartChTerm(NULL);
	upoint TermSize = GetTerminalSize();
	TermSize.y-=2;
	TermSize.x-=2;
	uint y = 1, x = 1;
	long k = 0;

	while (k!=KEY_q){
		move(TermSize.y+2, 0);
		printf("%u, %u", y, x);
		move(y+1, x+1); // move

		flush(); // and show

		k = GetChId();
		// status line
		switch (k) {
			case KEY_w:
				if (y) y--;
				break;
			case KEY_s:
				if (y < TermSize.y) y++;
				break;
			case KEY_a:
				if (x) x--;
				break;
			case KEY_d:
				if (x < TermSize.x) x++;
				break;
		}

	};

	StopChTerm(NULL);
}

