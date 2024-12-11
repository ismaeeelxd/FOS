#include "inc/lib.h"
struct semaphore create_semaphore(char *semaphoreName, uint32 value) {
	//TODO: [PROJECT'24.MS3 - #02] [2] USER-LEVEL SEMAPHORE - create_semaphore
	//COMMENT THE FOLLOWING LINE BEFORE START CODING
//	panic("create_semaphore is not implemented yet");
	//Your Code is Here...
	//struct __semdata semm;
	uint32 size = sizeof(struct __semdata);
	struct semaphore s;
//	cprintf("Creating semaphore : %s, with value : %d\n",semaphoreName,value);
	s.semdata=smalloc(semaphoreName,size,1);

	s.semdata->count=value;

	for(int i=0;i<64;i++)
	{
		s.semdata->name[i]=semaphoreName[i];
	}
	sys_init_queue(&(s.semdata->queue));
	s.semdata->lock=0;
//	cprintf("ana khlst awlll function\n");
	return s ;
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
//	panic("wait_semaphore is not implemented yet");
	//Your Code is Here...
	//uint32 k = 1;
//	sem.semdata->count--;
	sys_wait(&(sem));

}



void signal_semaphore(struct semaphore sem) {
	//TODO: [PROJECT'24.MS3 - #05] [2] USER-LEVEL SEMAPHORE - signal_semaphore
	//COMMENT THE FOLLOWING LINE BEFORE START CODING
	//panic("signal_semaphore is not implemented yet");
	//Your Code is Here...

	sys_signal(&(sem));

//	cprintf("khlst if cond");
	}



int semaphore_count(struct semaphore sem) {
	return sem.semdata->count;
}
