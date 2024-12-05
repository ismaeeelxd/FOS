// User-level Semaphore

#include "inc/lib.h"
struct semaphore create_semaphore(char *semaphoreName, uint32 value) {
	//TODO: [PROJECT'24.MS3 - #02] [2] USER-LEVEL SEMAPHORE - create_semaphore
	//COMMENT THE FOLLOWING LINE BEFORE START CODING
	panic("create_semaphore is not implemented yet");
	//Your Code is Here...
	//struct semaphore s;
	//init_spinlock()
}
struct semaphore get_semaphore(int32 ownerEnvID, char* semaphoreName) {
	//TODO: [PROJECT'24.MS3 - #03] [2] USER-LEVEL SEMAPHORE - get_semaphore
	//COMMENT THE FOLLOWING LINE BEFORE START CODING
	//panic("get_semaphore is not implemented yet");
	//Your Code is Here...
	struct semaphore sem;
	sem.semdata = sget(ownerEnvID, semaphoreName);
	return sem;
}
//WAAIIITT
void wait_semaphore(struct semaphore sem) {
	//TODO: [PROJECT'24.MS3 - #04] [2] USER-LEVEL SEMAPHORE - wait_semaphore
	//COMMENT THE FOLLOWING LINE BEFORE START CODING
	panic("wait_semaphore is not implemented yet");
	//Your Code is Here...
}

void signal_semaphore(struct semaphore sem) {
	//TODO: [PROJECT'24.MS3 - #05] [2] USER-LEVEL SEMAPHORE - signal_semaphore
	//COMMENT THE FOLLOWING LINE BEFORE START CODING
	//panic("signal_semaphore is not implemented yet");
	//Your Code is Here...
	uint32 keys = 1;
	while (keys != 0) {
		xchg(&keys, sem.semdata->lock);
	}
	sem.semdata->count++;
	if (sem.semdata->count <= 0) {
		struct Env_Queue* t = &(sem.semdata->queue);
		struct Env* e = sys_dequeue(t);
		sys_ready_enqueue(e);
	}
	sem.semdata->lock = 0;
}

int semaphore_count(struct semaphore sem) {
	return sem.semdata->count;
}
