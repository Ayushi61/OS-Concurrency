#include <xinu.h>

syscall initlock(lock_t* l)
{
	nlockCount++;
	if(nlockCount>NLOCKS)
		return SYSERR;
	l->lock=0; /* lock =0 implies lock is available, lock=1 implies lock is held*/
	l->guard=0;
	l->q=newqueue();
	return OK;
}

syscall lock(lock_t* l)
{
	
	while(test_and_set(&l->guard,1)==1)
		sleep(QUANTUM);  /*spin lock on guard*/
	if(l->lock==0)
	{
		l->lock=1;
		l->pid=currpid; 
		//kprintf("%d is acquiring lock \n",currpid);
		l->guard=0;
		
	}
	else
	{
		//kprintf("enqueue %d\n",currpid);
		enqueue(currpid,l->q);
		setpark();
		l->guard=0;
		park();
		l->pid=currpid;
		
	}
	
	return OK;
}
syscall unlock(lock_t* l)
{
	if(l->pid!=currpid)
	{
		////kprintf("system Error!");
		return SYSERR;
	}	
	//kprintf("inside unclock \n");
	while(test_and_set(&l->guard,1)==1)
		sleep(QUANTUM);  /*spin lock on guard*/
	//kprintf("guard =0\n");
	if(isempty(l->q))
	{
		
		l->lock=0;
		nlockCount-=1;
		//kprintf("unlocked\n");
	}	
	else
	{
		//l->lock=0;
		//kprintf("unparking \n");
		//l->pid=queuetab[queuehead(l->q)].qnext;
		unpark(dequeue(l->q));
	}
	l->guard=0;
	//kprintf("unlock Done of process %d!!!!!!!!!!!!!!!!!!!!!!!!!!\n",currpid); 
	return OK;
}


void park(lock_t *l)
{
	intmask	mask;			/* Saved interrupt mask		*/
	mask = disable();
	if(proctab[currpid].prpark==1)
	{
		proctab[currpid].prstate = PR_SLEEP;
		resched();
		
	}
	restore(mask);
}

void unpark(pid32 pid)
{
	intmask	mask;			/* Saved interrupt mask		*/
	mask = disable();
	//pid32 pid;
	//pid=dequeue(q);
	if(proctab[pid].prpark==1)
	{
		proctab[pid].prpark=0;
		//kprintf("unsetting park of process %d to 0 \n",pid);
	}
	proctab[pid].prstate = PR_READY;
	insert(pid, readylist, proctab[pid].prprio);
	//proctab[pid].prprio=5;
	//proctab[currpid].prprio=1;
	//kprintf("dequeueing pid %d\n",pid);
	//ready(pid);
	
	//kprintf("dequeueing pid %d\n",pid);
	restore(mask);
}

void setpark(void)
{
	intmask	mask;			/* Saved interrupt mask		*/
	mask = disable();
	//kprintf("setting park of process %d to 1 \n",currpid);
	proctab[currpid].prpark = 1;
	
	restore(mask);

}
