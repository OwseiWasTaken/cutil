#include "cutil.h"

int main(void) {
	StartChTerm(NULL);
	point TermSize = GetTerminalSize();
	int y = 2, x = 2;
	long k = 0;

	while (k!=KEY_q){
		move(y, x); // move
		flush(); // and show

		k = GetChId();
		// status line
		{
			move(TermSize.y, 0);
			printf("udlr: %i%i%i%i @ %i,%i",
				k==KEY_up, k==KEY_down, k==KEY_left, k==KEY_right,
				y, x
			);
		}
		switch (k) {
			case KEY_up:
				if (y-1) y--;
				break;
			case KEY_down:
				if (y < TermSize.y) y++;
				break;
			case KEY_left:
				if (x-1) x--;
				break;
			case KEY_right:
				if (x < TermSize.x) x++;
				break;
		}

	};

	StopChTerm(NULL);
}

