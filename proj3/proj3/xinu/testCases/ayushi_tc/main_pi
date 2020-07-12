/*  main.c  - main */

#include <xinu.h>



#define TC1_PI
#define TC2_PI

void sync_printf(char *fmt, ...)
{
        intmask mask = disable();
        void *arg = __builtin_apply_args();
        __builtin_apply((void*)kprintf, arg, 100);
        restore(mask);
}
process increment1(uint32 *x, uint32 n1, pi_lock_t *mutex1){
	uint32 i, j,k,l;	
	for (i=0; i<n1; i++){
		//sync_printf("thread %d with prio %d mutex1\n",currpid,proctab[currpid].prprio); 
		pi_lock(mutex1);
		(*x)+=1;
		sleepms(170);

	for (j=0; j<10000; j++);
		//sleep(QUANTUM);
		yield();
		pi_unlock(mutex1);
	}

	
	return OK;
}

process increment2(uint32 *x, uint32 n1, pi_lock_t *mutex1){
	uint32 i, j,k,l;	
	for (i=0; i<n1; i++){
		//sync_printf("thread %d with prio %d mutex1\n",currpid,proctab[currpid].prprio); 
		pi_lock(mutex1);
		(*x)+=1;
		sleepms(70);

	for (j=0; j<10000; j++);
		//sleep(QUANTUM);
		yield();
		pi_unlock(mutex1);
	}
	
	return OK;
}

process increment(uint32 *x,uint32 *y, uint32 n1,uint32 n2, pi_lock_t *mutex1,pi_lock_t *mutex2){
	uint32 i, j,k,l;	
	for (i=0; i<n1; i++){
		//sync_printf("thread %d with prio %d mutex1\n",currpid,proctab[currpid].prprio); 
		pi_lock(mutex1);
		(*x)+=1;
		sleepms(70);
		for (k=0; k<n2; k++){
			//sync_printf("thread %d with prio %d mutex2\n",currpid,proctab[currpid].prprio);
			pi_lock(mutex2);
			(*y)+=1;
			sleepms(100);
			//sync_printf("nested\n");
			for (l=0; l<10000; l++);
			//sleep(QUANTUM);
			yield();
			pi_unlock(mutex2);
		}
		for (j=0; j<10000; j++);
		//sleep(QUANTUM);
		yield();
		pi_unlock(mutex1);
	}
	
	return OK;
}

process nthreads(uint32 nt, uint32 *x,uint32 *y,uint32 n1,uint32 n2, pi_lock_t *mutex1,pi_lock_t *mutex2){
	pid32 pids[nt];
	int i;
	for (i=0; i < nt; i++){
		pids[i] = create((void *)increment, INITSTK, i+1,"p", 6, x,y ,n1,n2, mutex1,mutex2);
		if (pids[i]==SYSERR){
			kprintf("nthreads():: ERROR - could not create process");
			return SYSERR;
		}
	}
	for (i=0; i < nt; i++){
		if (resume(pids[i]) == SYSERR){
			kprintf("nthreads():: ERROR - could not resume process");
			return SYSERR;
		}
		sleepms(10);
	}
	
	//sleepms(10);
	for (i=0; i < nt; i++) receive();
	return OK;
}

process	main(void)
{
	
	 
	kprintf("\n\n=====       Testing the Priority inversion LOCK w/ sleep&guard         =====\n");
	uint32 x,y;			// shared variable
	unsigned nt;			// number of threads cooperating
	unsigned value1 = 4;// target value of variable
	unsigned value2 = 4;
	pi_lock_t mutex1;  			// lock
	pi_lock_t mutex2;

	#ifdef TC1_PI
	kprintf("\n\n================= TEST 1--> 3 threads (P6 prio1(L1,L2)->P7 prio2(L2)->P8 prio3(L1))===================\n");
	x = 0;y=0;	nt = 3;
	pid32 pids[nt];
 	pi_initlock(&mutex1); 
	pi_initlock(&mutex2); 
	//resume(create((void *)nthreads, INITSTK, 1,"nthreads", 7, nt, &x,&y, value1/nt,value2/nt, &mutex1, &mutex2));
	//receive(); 
	
	pids[0]=create((void *)increment, INITSTK, 1,"p", 6, &x,&y ,value1,value2, &mutex1,&mutex2);
	pids[1]=create((void *)increment1, INITSTK, 2,"p", 4, &y ,value2, &mutex2);
	pids[2]=create((void *)increment2, INITSTK, 3,"p", 4, &x ,value2, &mutex1);
	resume(pids[0]);
	sleepms(170);
	resume(pids[1]);
	sleepms(10);
	resume(pids[2]);	
	receive();
	receive();
	receive();
	sync_printf("%d threads, n=%d, target value=%d\n", nt, value1, x);
	sync_printf("%d threads, n=%d, target value=%d\n", nt, value2, y);
	

	#endif
	#ifdef TC2_PI

	kprintf("\n\n================= TEST 2--> 4 threads (P1-prio 1(L1,L2)-> p2 prio 4(L1,L2), p3 -prio 2(L2),p4- prio 3(L1))===================\n");
	x = 0;y=0;	nt = 4;
	pid32 pids1[nt];
 	pi_initlock(&mutex1); 
	pi_initlock(&mutex2); 
	//resume(create((void *)nthreads, INITSTK, 1,"nthreads", 7, nt, &x,&y, value1/nt,value2/nt, &mutex1, &mutex2));
	//receive(); 
	
	pids1[0]=create((void *)increment, INITSTK, 1,"p", 6, &x,&y ,value1,value2, &mutex1,&mutex2);
	pids1[1]=create((void *)increment, INITSTK, 4,"p", 6,&x, &y ,value1,value2,&mutex1,&mutex2);
	pids1[2]=create((void *)increment1, INITSTK, 2,"p", 4, &y ,value2, &mutex2);
	pids1[3]=create((void *)increment2, INITSTK, 3,"p", 4, &x ,value2, &mutex1);
	resume(pids1[0]);
	sleepms(170);
	
	
	resume(pids1[2]);
	sleepms(10);
	resume(pids1[3]);
	sleepms(10);
	resume(pids1[1]);	
	receive();
	receive();
	receive();
	receive();
	sync_printf("%d threads, n=%d, target value=%d\n", nt, value1, x);
	sync_printf("%d threads, n=%d, target value=%d\n", nt, value2, y);
	#endif
	//#endif
	return OK;
}
