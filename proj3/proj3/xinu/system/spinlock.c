#include <xinu.h>




syscall sl_initlock(sl_lock_t *l)
{
	spinCount++;
	if(spinCount>NSPINLOCKS)
		return SYSERR;
	l->lock=0; /* lock =0 implies lock is available, lock=1 implies lock is held*/
	return OK;
}

syscall sl_lock(sl_lock_t *l)
{
	//kprintf("lock is with %d\n",l->pid);
	//kprintf("value of l before test_and_spin is %d\n", l->lock);
	while(test_and_set(&l->lock,1)==1);  /*spin locl*/
	//kprintf("value of l after test and spin is %d\n",l->lock);
	l->pid=currpid; 
	return OK;
}
syscall sl_unlock(sl_lock_t *l)
{
	if(l->pid!=currpid)
		return SYSERR;
		
	l->lock=0; /*unlock*/
	spinCount--;
	return OK;
}
