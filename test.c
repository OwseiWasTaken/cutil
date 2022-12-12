#include "cutil.h"

int main(void) {
	StartChTerm(NULL);

	// write NULL
	kbkey k = GetChId();
	printf("is up: %s\n", k==KEY_up?"true":"false");
	printf("is down: %s\n", k==KEY_down?"true":"false");

	StopChTerm(NULL);
}
