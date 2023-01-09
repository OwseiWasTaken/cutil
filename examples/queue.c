#include "cutil.h"

int main(int argc, char *argv[]) {
	argv++; argc--;

	queue_t q = MakeQueue();

	srand(time(NULL));
	int ThreadId = rand();
	QueueAppend(&q, ThreadId);
	QueueAppend(&q, randint(1, 6));

	int nextid = QueuePop(&q);

	printf("%d-> %d\n", ThreadId, nextid);
	int rnd = QueuePop(&q);
	ReallocQueue(&q);
	printf("rand -> %d\n", rnd);
	return 0;
}

