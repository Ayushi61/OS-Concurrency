/*lock.h*/

#define NSPINLOCKS  20  /* max number of spin locks*/
#define NLOCKS  20  /* max number of  locks*/
#ifndef NPILOCKS
#define NPILOCKS 20
#endif
#define NALOCKS 20
extern uint32 spinCount;
extern uint32 nlockCount;
extern uint32 pi_nlockCount;
extern uint32 al_nlock;


