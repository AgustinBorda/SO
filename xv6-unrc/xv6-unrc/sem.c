#include "defs.h"
#include "param.h"
#include "sem.h"
#include "spinlock.h"

// The table of all the semaphores in the system.
// all accesses must ensure mutual exclusion
// A space s is unused when s.refcount == 0
struct {
  spinlock lock;
  struct sem sems[NOSEM];
} semtable;


// Allocates a semaphore s in the osem array 
// of a process p.
// returns the position where s is allocated if success,
// otherwise returns -1
int
allocsem(struct proc* p, struct sem* s)
{
  uint i = 0;
  while (i < NOSEM && p->osem[i])
    i++;

  if (i == NOSEM)
    return -1;

  p->osem[i] = s;
  return i;
}


// Creates a new semaphore if not exists
// a semaphore with the given key,
// otherwise gets the semaphore with the key.
// if sucess returns the descriptor of the semaphore in myproc() -> osem and increments refcount by 1
// or returns -1 if an error ocurred (invalid init_value or no semaphores available)
// Must be called with the lock of the semtable acquired.   
int static
semget1(int key, int init_value)
{
  sem* s = 0;
  int i = 0;

  for (i = 0; i < NOSEM; i++) {

    if (semtable.sems[i].key == key) {
      s = &(semtable.sems[i]);            // If encounter a semaphore with the same key
      break;                              // There is no point on keep searching
    }

    if (!(semtable.sems[i].refcount))
      s = &(semtable.sems[i]);
  }

  if (!s) 
    return -1;

  if (i == NOSEM) {
    s->value = init_value;
    s->key = key;
    initlock(s->lock, (char*) key);
  }

  acquire(&(s->lock)); // If s is a existent lock, we must ensure mutual exclusion with refcount
  s->refcount++;
  release(&(s->lock));

  return allocsem(myproc(), s);
}

// Calls semget1 with the lock of the semtable acquired,
// releases it after use.
int
semget(int key, int init_value)
{
  acquire(&semtable.lock);
  int res = semget1(key, init_value);
  release(&semtable.lock);
  return res;
}

// Removes a semaphore from myproc() -> osem
// and decrements refcount by 1.
// returns 0 if the semaphore with semid exists, otherwise 
// returns -1
int
semclose(int semid)
{
  struct proc* p = myproc();

  if (!p->osem[semid])
    return -1;
  
  acquire(&(p->osem[semid].lock));   // We must ensure mutual exclusion with refcount
  p->osem[semid].refcount--;
  release(&(p->osem[semid].lock));

  p->osem[semid] = 0;
  return 0;
}

// Increment the value of the semaphore by 1.
// all the accesses to value must ensure mutual exclusion.
int
semup(int semid)
{
  struct proc* p = myproc();
  acquire(&(p->osem[semid].lock));
  p->osem[semid].value++;
  release(&(p->osem[semid].lock));
  wakeup(&(p->osem[semid].lock));
  return 0;
}

// Gets the value of a semaphore 
// ensuring mutual exclusion
int
mutualexclusionvalue(int semid)
{
  struct proc* p = myproc();
  acquire(&(p->osem[semid].lock));
  int res = p->osem[semid].value;
  release(&(p->osem[semid].lock));
  return res;
}

// Decrement the value of the semaphore by 1 if value > 0,
// otherwise sleeps the invocant proc.
// all the accesses to value must unsure mutual exclusion
// The process p sleeps on chan &(p->osem[semid].lock)
int
semdown(int semid)
{ 
  struct proc* p = myproc();
  while (!mutualexclusionvalue(semid))
    sleep(&(p->osem[semid].lock), &(p->osem[semid].lock));
  
  acquire(&(p->osem[semid].lock));
  p->osem[semid].value--;
  release(&(p->osem[semid].lock));
  
  return 0;
}

