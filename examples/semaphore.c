#include "cutil.h"

typedef	queue_t sem_t;

#define SEMSLEEP 0.3

// look and wait
void execute (sem_t *sem, int ThreadId) {
	QueueAppend(sem, ThreadId);
	if (QueueSize(sem)-1) {
		do {
			fsleep(SEMSLEEP);
			if (QueueRead(sem) == ThreadId) {
				QueuePop(sem);
				return;
			}
		} while (1);
	}
}

// finished 'crossing'
void release (sem_t *sem, int ThreadId) {
	assert(QueuePop(sem) == ThreadId);
}

int main( void ) {
	sem_t *sem = malloc(sizeof(sem_t));
	InitQueue(sem);

	if (0){ // this works
		execute(sem, 1); // thread 1 blocks
		release(sem, 1); // thread 1 relseases
		execute(sem, 2); // thread 2 blocks
		release(sem, 2); // thread 2 releases
		_P_queue(sem);
	}

	if (0){ // this waits forever (since single thread)
		execute(sem, 1); // thread 1 blocks
		execute(sem, 2); // thread 2 is blocked
		// since single threaded, 'thread 1' is also blocked
		release(sem, 1); // thread 1 never releases, since all are blocked
		release(sem, 2); // and so does thread 2
		_P_queue(sem);
	}

	if (0){ // this doesn't wait forever (assuming multi-threading)
		execute(sem, 1); // thread 1 passed
		execute(sem, 2); // thread 2 waiting
		release(sem, 1); // thread 1 released
		// thread 2 got released from wait loop in execute
		release(sem, 2); // thread 2 released
		_P_queue(sem);
	}

	return 0;
}

