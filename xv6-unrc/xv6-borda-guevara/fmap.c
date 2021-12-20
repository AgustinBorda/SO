#include "types.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "x86.h"
#include "proc.h"
#include "fmap.h"
#include "spinlock.h"
#include "sleeplock.h"
#include "fs.h"
#include "file.h"
#include "stat.h"

// Defines if a value (val) is between a base address and
// an offset.
#define between(base, off, val) ((val >= base) && (val < base + off))

// Min of two values
#define min(a, b) ((a<=b) ? a : b)

// System fmap table.
struct {
  struct spinlock lock;
  struct fmap fmaps[NFMAP];
} fmaptable;

// Returns the index of a requested fmap
int
seek(uint addr, struct proc *p)
{
  int i = 0;
  while(i < NOFMAP && (!p->ofmap[i] || (p->ofmap[i] && !between(p->ofmap[i]->baseaddr, p->ofmap[i]->size, addr))))
    i++;

  if(i == NOFMAP)
    return -1;

  return i;
}

// Allocate the fmap into the ofmap array
int 
allocfmap(struct proc *p, struct fmap *fm)
{
  uint i = 0;
  while(i<NOFMAP && p->ofmap[i])
    i++;
  
  if(i == NOFMAP)
    return -1;

  p->ofmap[i] = fm;
 
  return p->ofmap[i]->baseaddr;
}

// Creates a fmap
struct fmap static
createfmap(struct file *f, uint baseaddr, uint endaddr)
{
  struct fmap fm;
  fm.baseaddr = baseaddr;
  fm.size = endaddr - baseaddr;
  fm.readable = f->readable;
  fm.writable = f->writable;
  fm.ip = f->ip;
  fm.present = 1;
  return fm;
}

// Maps a file into memory, caller
// must hold fmaptable.lock
int static 
mmap1(int fd)
{
  struct proc *p = myproc();
  struct file *f = p->ofile[fd];
  if (f->type == FD_INODE && f->readable) {
    uint i = 0;
    while(i < NFMAP && fmaptable.fmaps[i].present)
      i++;
  
    if(i == NFMAP)
      return -1;

    
    struct stat fs;
    filestat(f, &fs);
    uint baseaddr = PGROUNDDOWN(p->sz) + PGSIZE;
    p->sz = baseaddr + fs.size;
    struct fmap *fm = &(fmaptable.fmaps[i]);
    *fm = createfmap(f, baseaddr, p->sz);

    // Reserve all the page, making possible
    // a efficient unmapping
    p->sz = PGROUNDDOWN(p->sz) + PGSIZE;    

    return allocfmap(p, fm);

  }
  return -1;
}

// Calls mmap1 holding the locks
int
mmap(int fd)
{
  acquire(&fmaptable.lock);
  int res = mmap1(fd); 
  release(&fmaptable.lock);
  return res;
}

// Updates the dirty pages to the mapped file
void static
syncfmap(struct fmap *fm, struct proc *p)
{
  uint i = fm->baseaddr;
  uint endaddr = fm->baseaddr + fm->size;
  while(i < endaddr) {
    uint off = min(PGSIZE, (endaddr - fm->baseaddr)- PGROUNDDOWN(i));
    if(ispagewritable(p->pgdir, (char*) i)) {
      begin_op();
      ilock(fm->ip);
      writei(fm->ip, (char*) PGROUNDDOWN(i), i-fm->baseaddr, off);
      iunlock(fm->ip);
      end_op();
    }
    i += off;
  }
}

// Removes a mmap from the memory
// Also, deallocates the memory used by the map and
// calls syncfmap
int 
munmap(char *addr)
{
  struct proc *p = myproc();
  int i = seek((uint) addr, p);
  if(i >= 0) {

    if(p->ofmap[i]->writable)
      syncfmap(p->ofmap[i], p);

    deallocuvm(p->pgdir, p->ofmap[i]->baseaddr, p->ofmap[i]->baseaddr + p->ofmap[i]->size);  

    acquire(&fmaptable.lock);
    p->ofmap[i]->present = 0;
    release(&fmaptable.lock);
    p->ofmap[i] = 0;

    return 0;
  }
  return -1;
}











