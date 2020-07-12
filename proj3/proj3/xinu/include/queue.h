/* queue.h - firstid, firstkey, isempty, lastkey, nonempty		*/

/* Queue structure declarations, constants, and inline functions	*/

/* Default # of queue entries: 1 per process plus 2 for ready list plus	*/
/*			2 for sleep list plus 2 per semaphore		*/
#ifndef NQENT
#define NQENT	(NPROC + 4 + NSEM + NSEM+20)
#endif
#ifndef NPILOCKS
#define NPILOCKS 20
#endif
#define	EMPTY	(-1)		/* Null value for qnext or qprev index	*/
#define	MAXKEY	0x7FFFFFFF	/* Max key that can be stored in queue	*/
#define	MINKEY	0x80000000	/* Min key that can be stored in queue	*/

struct	qentry	{		/* One per process plus two per list	*/
	int32	qkey;		/* Key on which the queue is ordered	*/
	qid16	qnext;		/* Index of next process or tail	*/
	qid16	qprev;		/* Index of previous process or head	*/
};

extern	struct qentry	queuetab[];
//extern 	struct qentry	proc_locks;
/* Inline queue manipulation functions */

#define	queuehead(q)	(q)
#define	queuetail(q)	((q) + 1)
#define	firstid(q)	(queuetab[queuehead(q)].qnext)
//#define	firstid_lockq()	(proc_locks.qnext)
#define	lastid(q)	(queuetab[queuetail(q)].qprev)
//#define	lastid_lockq()	(proc_locks.qprev)
#define	isempty(q)	(firstid(q) >= NPROC)
//#define isempty_lockq()	(firstid_lockq() >= NPILOCKS)
#define	nonempty(q)	(firstid(q) <  NPROC)
//#define nonempty_lockq()	(firstid_lockq() < NPILOCKS)
#define	firstkey(q)	(queuetab[firstid(q)].qkey)
//#define firstkey_lockq()	(firstid_lockq().qkey)
#define	lastkey(q)	(queuetab[ lastid(q)].qkey)
//#define firstkey_lockq()	(lastid_lockq().qkey)

/* Inline to check queue id assumes interrupts are disabled */

#define	isbadqid(x)	(((int32)(x) < NPROC) || (int32)(x) >= NQENT-1)
