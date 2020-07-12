#include <xinu.h>
pri16 al_restoreq(pid32 pid,qid16 q);
syscall al_initlock(al_lock_t* l)
{
	al_nlock++;
	if(al_nlock>NALOCKS)
		return SYSERR;
	l->lock=0; /* lock =0 implies lock is available, lock=1 implies lock is held*/
	l->guard=0;
	l->q=newqueue();
	l->prio=1;
	return OK;
}

syscall al_lock(al_lock_t* l)
{
	
	while(test_and_set(&l->guard,1)==1)
		sleep(QUANTUM);  /*spin lock on guard*/
	if(l->lock==0)
	{
		l->lock=1;
	
		struct procent *curr_prptr;
		curr_prptr=&proctab[currpid];
		curr_prptr->pr_al_locks[curr_prptr->pr_tot_locks]=l;
		curr_prptr->pr_tot_locks+=1;
		l->prio=curr_prptr->prprio;
		
		l->pid=currpid; 
		//kprintf("%d is acquiring lock \n",currpid);
		l->guard=0;
		
	}
	else
	{
		//kprintf("enqueue %d\n",currpid);
		//kprintf("checking for deadlock\n");
		//kprintf("%d\n",l);
		int deadlock=check_deadlock(currpid,l);
		if(!deadlock)
		{
			//kprintf("no deadlock\n");
			enqueue(currpid,l->q);
			l->prio=al_restoreq(currpid,l->q);
			al_setpark();
			l->guard=0;
			al_park();
			l->pid=currpid;
			pri16 prio=al_restoreq(l->pid,l->q);
			l->prio=prio;
			proctab[l->pid].pr_al_locks[proctab[l->pid].pr_tot_locks]=l;
			proctab[l->pid].pr_tot_locks+=1;
		}
		else
		{
			kprintf("\n");
			struct procent *curr_prptr;
			curr_prptr=&proctab[currpid];
			curr_prptr->deadlock_det+=1;
			l->guard=0;
			return SYSERR;
		}
	}
	
	return OK;
}
syscall al_unlock(al_lock_t* l)
{
	if(l->pid!=currpid)
	{
		kprintf("system Error!\n");
		return SYSERR;
	}	
	//kprintf("inside unclock \n");
	while(test_and_set(&l->guard,1)==1)
		sleep(QUANTUM);  /*spin lock on guard*/
	//kprintf("guard =0\n");
	struct procent *prptr;
	prptr=&proctab[l->pid];
	int i,k;
	/*remove locks from the list of locks of the process */
	int flag=0;
	////kprintf("total locks held by %d is %d\n",l->pid,prptr->pr_tot_locks);
	for(i=0;i<prptr->pr_tot_locks;i++)
	{
		if(prptr->pr_al_locks[i]==l)
		{
			for(k=i;k<prptr->pr_tot_locks-1;k++)
			{
				prptr->pr_al_locks[k]=prptr->pr_al_locks[k+1];
			}
			flag=1;
		}
	}
	if(flag==1)
		prptr->pr_tot_locks-=1;
	if(isempty(l->q))
	{
		l->lock=0;
		al_nlock-=1;
		//kprintf("unlocked\n");
	}	
	else
	{
		//l->lock=0;
		//kprintf("unparking \n");
		//l->pid=queuetab[queuehead(l->q)].qnext;
		al_unpark(dequeue(l->q));
	}
	l->guard=0;
	//kprintf("unlock Done of process %d!!!!!!!!!!!!!!!!!!!!!!!!!!\n",currpid); 
	return OK;
}

bool8 al_trylock(al_lock_t *l)
{
	if(l->lock==1)
	{
		//kprintf("lock is acquired %d\n",currpid);
		return 1;
	}
	else
	{
		//kprintf("acquiring lock %d\n",currpid);
		al_lock(l);
	}
		
	return 0;
}


void al_park()
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

void al_unpark(pid32 pid)
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

void al_setpark(void)
{
	intmask	mask;			/* Saved interrupt mask		*/
	mask = disable();
	//kprintf("setting park of process %d to 1 \n",currpid);
	proctab[currpid].prpark = 1;
	
	restore(mask);

}


pri16 al_restoreq(pid32 pid,qid16 q)
{
	struct procent *prptr;
	prptr=&proctab[pid];
	int i;
	pri16 max= prptr->prioinit;
	pid32 next;
	//for(i=0;i<prptr->pr_tot_locks;i++)
	//{
	//	pi_lock_t *l = prptr->pr_al_locks[prptr->pr_tot_locks];
	//	if (l->prio > max)
	//		max=l->prio;
	//}
	if(nonempty(q))
	{
		pri16 max2=proctab[firstid(q)].prprio;
		////kprintf("lastid of q is %d\n",lastid(q));
		////kprintf("firstid of q is %d \n",firstid(q));
		if (max2>max)
			max=max2;
		if(lastid(q)!=firstid(q))
		{
			next=firstid(q);
			if(max2>max)
				max=max2;
			while(next!=lastid(q))
			{
				
				////kprintf("prio of %d is %d\n",next,max2);
				next=queuetab[next].qnext;				
				max2=proctab[next].prprio;
				if(max2>max)
					max=max2;
				

			}
		}
		////kprintf("max2 is %d$$$$$\n",max2);
		//kprintf("max is %d \n",max);
	}
	//kprintf("restoring prio from %d to %d\n",prptr->prprio,max);
	return max;
}

int check_deadlock(pid32 pid,al_lock_t *l)
{
	struct procent *prptr;
	prptr=&proctab[pid];
	al_lock_t *al_l;
	pid32 p;
	int deadlock,i;
	//kprintf("entered deadlock detection\n");
	for(i=0;i<prptr->pr_tot_locks;i++)
	{
		al_l=prptr->pr_al_locks[i];
		if(al_l==l)

		{
			kprintf("deadlock detected=P%d",pid);			
			return 1;
		}
		if(nonempty(al_l->q))
		{
			//kprintf("nonempty recursion\n");
			p=firstid(al_l->q);
			do
			{	
				
				//kprintf("al_l=%d and p =%d and l=%d\n",al_l,p,l);
				deadlock=check_deadlock(p,l);
				if(deadlock!=1)
					if(p!=lastid(al_l->q))
						p=queuetab[p].qnext;
					else
						return 0;
				else 
				{
					kprintf("-P%d",pid);	
					return deadlock;
				}
			}while(p!=lastid(al_l->q));

		}
		else
			return 0;
		
	}
}
