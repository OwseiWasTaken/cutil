#include "cutil.h"

int main(void) {
	StartChTerm(NULL);
	upoint TermSize = GetTerminalSize();
	TermSize.y--;
	TermSize.x--;
	uint y = 1, x = 1;
	long k = 0;

	while (k!=KEY_q){
		move(TermSize.y+1, 0);
		printf("%u, %u", y, x);
		move(y+1, x+1); // move
		flush(); // and show

		k = GetChId();
		// status line
		switch (k) {
			case KEY_up:
				if (y) y--;
				break;
			case KEY_down:
				if (y < TermSize.y) y++;
				break;
			case KEY_left:
				if (x) x--;
				break;
			case KEY_right:
				if (x < TermSize.x) x++;
				break;
		}

	};

	StopChTerm(NULL);
}

