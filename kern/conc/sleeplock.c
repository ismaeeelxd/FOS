// Sleeping locks

#include "inc/types.h"
#include "inc/x86.h"
#include "inc/memlayout.h"
#include "inc/mmu.h"
#include "inc/environment_definitions.h"
#include "inc/assert.h"
#include "inc/string.h"
#include "sleeplock.h"
#include "channel.h"
#include "../cpu/cpu.h"
#include "../proc/user_environment.h"

void init_sleeplock(struct sleeplock *lk, char *name)
{
	init_channel(&(lk->chan), "sleep lock channel");
	init_spinlock(&(lk->lk), "lock of sleep lock");
	strcpy(lk->name, name);
	lk->locked = 0;
	lk->pid = 0;
}
int holding_sleeplock(struct sleeplock *lk)
{
	int r;
	acquire_spinlock(&(lk->lk));
	r = lk->locked && (lk->pid == get_cpu_proc()->env_id);
	release_spinlock(&(lk->lk));
	return r;
}
//==========================================================================
//+++++++++++++++ helper Funcation for Aacquire_sleeplock +++++++++++++++++++++++

void reHandleTheProccess(struct sleeplock *lk){
	lk->locked = 1;
		lk->pid = get_cpu_proc()->env_id;
}
void checkkinggoingToSleepFun(struct sleeplock *lk){
	while(lk->locked == 1){
			sleep(&(lk->chan),&(lk->lk));
		}
}
//+++++++++++++++ End helper Funcation for Aacquire_sleeplock +++++++++++++++++++++++

void acquire_sleeplock(struct sleeplock *lk)
{
	//TODO: [PROJECT'24.MS1 - #13] [4] LOCKS - acquire_sleeplock
	acquire_spinlock(&(lk->lk));
	checkkinggoingToSleepFun(lk);
	reHandleTheProccess(lk);

	release_spinlock(&(lk->lk));

}
//+++++++++++++++ helper Funcation for release_sleeplock +++++++++++++++++++++++

void resetTheProccess(struct sleeplock *lk){
	lk->locked = 0;
			lk->pid = 0;
}

void checkingTheQueueWaiting(struct sleeplock *lk){
	if(queue_size(&(lk->chan.queue)) != 0){
				wakeup_all(&(lk->chan));
			}
}
//+++++++++++++++ End helper Funcation for release_sleeplock +++++++++++++++++++++++

void release_sleeplock(struct sleeplock *lk)
{
	//TODO: [PROJECT'24.MS1 - #14] [4] LOCKS - release_sleeplock
	//COMMENT THE FOLLOWING LINE BEFORE START CODING

	int holding = holding_sleeplock(lk);
	if(holding){
		acquire_spinlock(&(lk->lk));
		checkingTheQueueWaiting(lk);

		resetTheProccess(lk);
		release_spinlock(&(lk->lk));
	}
	else{
		panic("this process does not hold the lock\n");
	}




	}







