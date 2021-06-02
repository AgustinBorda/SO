#include "defs.h"
#include "param.h"
#include "sem.h"
#include "spinlock.h"

// The table of all the semaphores in the system.
// all accesses must ensure mutual exclusion
struct {
  spinlock lock;
  struct sem sems[NOSEM];
} semtable;


// Creates a new semaphore if not exists
// a semaphore with the given key,
// otherwise gets the semaphore with the key.
// if sucess returns the descriptor of the semaphore in myproc() -> osem and increments refcount by 1
// or returns -1 if an error ocurred (invalid init_value or no semaphores available)   
int
semget(int key, int init_value)
{
return 0; //TODO: Implement
}

// Removes a semaphore from myproc() -> osem
// and decrements refcount by 1.
// if refcount == 0, also removes the semaphore from the semtable
// returns 0 if the semaphore with semid exists, otherwise 
// returns -1
int
semclose(int semid)
{
return 0; //TODO: Implement
}

// Increment the value of the semaphore by 1.
// all the accesses to value must ensure mutual exclusion.
int
semup(int semid)
{
return 0; //TODO: Implement
}

// Decrement the value of the semaphore by 1 if value > 0,
// otherwise sleeps the invocant proc.
// all the accesses to value must unsure mutual exclusion
// the process sleeping sleeps on the chan semid.
int
semdown(int semid)
{
return 0; //TODO: Implement
}

