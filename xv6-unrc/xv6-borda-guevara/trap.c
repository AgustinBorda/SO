#include "types.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "x86.h"
#include "traps.h"
#include "spinlock.h"
#include "fmap.h"
#include "sleeplock.h"
#include "fs.h"
#include "file.h"

#define min(a, b) ((a<=b) ? a : b)

// Interrupt descriptor table (shared by all CPUs).
struct gatedesc idt[256];
extern uint vectors[];  // in vectors.S: array of 256 entry pointers
struct spinlock tickslock;
uint ticks;

void
tvinit(void)
{
  int i;

  for(i = 0; i < 256; i++)
    SETGATE(idt[i], 0, SEG_KCODE<<3, vectors[i], 0);
  SETGATE(idt[T_SYSCALL], 1, SEG_KCODE<<3, vectors[T_SYSCALL], DPL_USER);

  initlock(&tickslock, "time");
}

void
idtinit(void)
{
  lidt(idt, sizeof(idt));
}

void
killproc(struct trapframe *tf)
{
  cprintf("pid %d %s: trap %d err %d on cpu %d "
          "eip 0x%x addr 0x%x--kill proc\n",
           myproc()->pid, myproc()->name, tf->trapno,
           tf->err, cpuid(), tf->eip, rcr2());
  myproc()->killed=1;
}

//PAGEBREAK: 41
void
trap(struct trapframe *tf)
{
  if(tf->trapno == T_SYSCALL){
    if(myproc()->killed)
      exit();
    myproc()->tf = tf;
    syscall();
    if(myproc()->killed)
      exit();
    return;
  }

  switch(tf->trapno){
  case T_IRQ0 + IRQ_TIMER:
    if(cpuid() == 0){
      acquire(&tickslock);
      ticks++;
      wakeup(&ticks);
      release(&tickslock);
    }
    lapiceoi();
    break;
  case T_IRQ0 + IRQ_IDE:
    ideintr();
    lapiceoi();
    break;
  case T_IRQ0 + IRQ_IDE+1:
    // Bochs generates spurious IDE1 interrupts.
    break;
  case T_IRQ0 + IRQ_KBD:
    kbdintr();
    lapiceoi();
    break;
  case T_IRQ0 + IRQ_COM1:
    uartintr();
    lapiceoi();
    break;
  case T_IRQ0 + 7:
  case T_IRQ0 + IRQ_SPURIOUS:
    cprintf("cpu%d: spurious interrupt at %x:%x\n",
            cpuid(), tf->cs, tf->eip);
    lapiceoi();
    break;

  //PAGEBREAK: 13
  default:
    if(myproc() == 0 || (tf->cs&3) == 0){
      // In kernel, it must be our mistake.
      cprintf("unexpected trap %d from cpu %d eip %x (cr2=0x%x)\n",
              tf->trapno, cpuid(), tf->eip, rcr2());
      panic("trap");
    }
    // In user space, check if the direction that fired the 
    // page fault is valid (between the gap and the size of the process).
    // If its valid, allocate another page for the dir, else the process misbehaved.
    if(myproc()->stackgap <= rcr2() && myproc()->sz >= rcr2()) {
  
      // If the page isn't present, allocate it,
      // else the fault was happened because the page is read only or the process
      // misbehabed 
      if(!ispagepresent(myproc()->pgdir, (char*)PGROUNDDOWN(rcr2()))) {
        allocuvm(myproc()->pgdir, PGROUNDDOWN(rcr2()), PGROUNDDOWN(rcr2()+PGSIZE));
      
        // If the fault occured in addresses greater than the bottom of the stack
        // check if the fault occurred in the range of a fmap of the process
        // if so, allocate a a page and loads it  with the information of the mapped file
        // else, the process misbehaved. 
        if(rcr2() > myproc()->stackbase) {
          
          int i = seek(rcr2(), myproc());

          if(i >= 0) {
            char *dest = (char*) PGROUNDDOWN(rcr2());
            uint off = PGROUNDDOWN(rcr2()) - myproc()->ofmap[i]->baseaddr;
            uint n = min(PGSIZE, myproc()->ofmap[i]->size - off);
            ilock(myproc()->ofmap[i]->ip);
            readi(myproc()->ofmap[i]->ip, dest, off, n); 
            iunlock(myproc()->ofmap[i]->ip); 
            // Make the page read-only, this is to 
            // use the PTE_W bit as a dirty flag
            clearptew(myproc()->pgdir, (char*)PGROUNDDOWN(rcr2()));
          }
          else {
            killproc(tf);
          }
        }
      }
      else {
        // If the page is in user space, is present and caused
        // a page-fault, must be a read only page being written.
        // If is in a fmap address, make the page writable,
        // else, the process misbehaved.
        if(rcr2() > myproc()->stackbase && seek(rcr2(), myproc()))
          setptew(myproc()->pgdir, (char*) PGROUNDDOWN(rcr2()));
        else
          killproc(tf);
      }
    }
    else {
      killproc(tf);
    }
  }

  // Force process exit if it has been killed and is in user space.
  // (If it is still executing in the kernel, let it keep running
  // until it gets to the regular system call return.)
  if(myproc() && myproc()->killed && (tf->cs&3) == DPL_USER)
    exit();

  // Force process to give up CPU on clock tick.
  // If interrupts were on while locks held, would need to check nlock.
  if(myproc() && myproc()->state == RUNNING &&
     tf->trapno == T_IRQ0+IRQ_TIMER && myproc()->remainingticks-- == 0)
    yield();

  // Check if the process has been killed since we yielded
  if(myproc() && myproc()->killed && (tf->cs&3) == DPL_USER)
    exit();
}
