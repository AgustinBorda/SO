#include "types.h"
#include "x86.h"
#include "defs.h"
#include "date.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"


int
sys_fork(void)
{
  return fork();
}

int
sys_exit(void)
{
  exit();
  return 0;  // not reached
}

int
sys_wait(void)
{
  return wait();
}

int
sys_kill(void)
{
  int pid;

  if(argint(0, &pid) < 0)
    return -1;
  return kill(pid);
}

int
sys_getpid(void)
{
  return myproc()->pid;
}

int
sys_sbrk(void)
{
  int addr;
  int n;

  if(argint(0, &n) < 0)
    return -1;
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

int
sys_sleep(void)
{
  int n;
  uint ticks0;

  if(argint(0, &n) < 0)
    return -1;
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(myproc()->killed){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

// return how many clock tick interrupts have occurred
// since start.
int
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

int
sys_set_priority(void) 
{
  int priority;
  argint(0, &priority);
  return set_priority(priority);
}

int
sys_semget(void)
{
  int key;
  int val;

  argint(0, &key);
  argint(1, &val);

  return semget(key, val);  
}

int
sys_semclose(void)
{
  int semid;

  argint(0, &semid);

  return semclose(semid);
}

int
sys_semup(void)
{
  int semid;

  argint(0, &semid);

  return semup(semid);
}

int
sys_semdown(void)
{
  int semid;

  argint(0, &semid);

  return semdown(semid);
}

int
sys_mmap(void)
{
  int fd;
  
  argint(0, &fd);

  return mmap(fd);
}

