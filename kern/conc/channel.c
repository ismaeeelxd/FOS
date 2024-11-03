/*
 * channel.c
 *
 *  Created on: Sep 22, 2024
 *      Author: HP
 */
#include "channel.h"
#include <kern/proc/user_environment.h>
#include <kern/cpu/sched.h>
#include <inc/string.h>
#include <inc/disk.h>

//===============================
// 1) INITIALIZE THE CHANNEL:
//===============================
// initialize its lock & queue
void init_channel(struct Channel *chan, char *name)
{
	strcpy(chan->name, name);
	init_queue(&(chan->queue));
}

//===============================
// 2) SLEEP ON A GIVEN CHANNEL:
//===============================
// Atomically release lock and sleep on chan.
// Reacquires lock when awakened.
// Ref: xv6-x86 OS code
//+++++++++++++++ helper Funcation for Sleep+++++++++++++++++++++++
void reacquire_spinlock(struct spinlock* lk){
	acquire_spinlock(lk);

}

void setTheProccessBlocked(struct Channel *chan){
	struct Env* curr_proc = get_cpu_proc();
		curr_proc->env_status = ENV_BLOCKED;
		enqueue(&(chan->queue),curr_proc);
}
//+++++++++++++++ End helper Funcation for Sleep+++++++++++++++++++++++

void sleep(struct Channel *chan, struct spinlock* lk)
{
	//TODO: [PROJECT'24.MS1 q- #10] [4] LOCKS - sleep
	acquire_spinlock(&ProcessQueues.qlock);
	release_spinlock(lk);
	setTheProccessBlocked(chan);
	sched();
	reacquire_spinlock(lk);
	release_spinlock(&ProcessQueues.qlock);

}

//==================================================
// 3) WAKEUP ONE BLOCKED PROCESS ON A GIVEN CHANNEL:
//==================================================
// Wake up ONE process sleeping on chan.
// The qlock must be held.
// Ref: xv6-x86 OS code
// chan MUST be of type "struct Env_Queue" to hold the blocked processes



//+++++++++++++++ helper Funcation for Wakeup+++++++++++++++++++++++

void makeWaitedElementReady(struct Channel *chan){
	struct Env *current_p;
	current_p = dequeue(&chan->queue);
			sched_insert_ready0(current_p);
}


//+++++++++++++++ End helper Funcation for Wakeup+++++++++++++++++++++++

void wakeup_one(struct Channel *chan)
{
	//TODO: [PROJECT'24.MS1 - #11] [4] LOCKS - wakeup_one
	acquire_spinlock(&ProcessQueues.qlock);
	struct Env *current_p;
	int size=queue_size(&chan->queue);
	if(size > 0){
		makeWaitedElementReady(chan);
	}
	release_spinlock(&ProcessQueues.qlock);

}

//====================================================
// 4) WAKEUP ALL BLOCKED PROCESSES ON A GIVEN CHANNEL:
//====================================================
// Wake up all processes sleeping on chan.
// The queues lock must be held.
// Ref: xv6-x86 OS code
// chan MUST be of type "struct Env_Queue" to hold the blocked processes

void wakeup_all(struct Channel *chan)
{
	//TODO: [PROJECT'24.MS1 - #12] [4] LOCKS - wakeup_all
	//COMMENT THE FOLLOWING LINE BEFORE START CODING
	acquire_spinlock(&ProcessQueues.qlock);
	struct Env *current_p;
	int size = queue_size(&chan->queue);
	for(int i=0;i<size;i++){
		makeWaitedElementReady(chan);

	}

	release_spinlock(&ProcessQueues.qlock);
}

