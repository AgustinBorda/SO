#include "types.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "cpu.h"
#include "x86.h"
#include "traps.h"
#include "spinlock.h"

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

//PAGEBREAK: 41
void
trap(struct trapframe *tf)
{
    switch(tf->trapno){
      case T_IRQ0 + IRQ_TIMER:
        if(cpuid() == 0){
          acquire(&tickslock);
          ticks++;
          if (ticks % 100 == 0)
              cprintf("Ticks: %d\n", ticks);
          release(&tickslock);
        }
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
        if( (tf->cs & 3) == 0){
          // In kernel, it must be our mistake.
          cprintf("unexpected trap %d from cpu %d eip %x (cr2=0x%x)\n",
                  tf->trapno, cpuid(), tf->eip, rcr2());
          panic("trap");
        }        
      } 
}
