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

#define inemptyfile(base, off, val) ((val==base) && (off == 0))

// Min of two values
#define min(a, b) ((a<=b) ? a : b)

// Returns the index of a requested fmap
int
seek(uint addr, struct proc *p)
{
  int i = 0;
  while(i < NOFMAP && (!p->ofmap[i].present || (p->ofmap[i].present && 
      (!between(p->ofmap[i].baseaddr, p->ofmap[i].size, addr) && !inemptyfile(p->ofmap[i].baseaddr, p->ofmap[i].size, addr))))){
    i++;
  }

  if(i == NOFMAP)
    return -1;
  return i;
}

// Creates a fmap
struct fmap static
createfmap(struct file *f, uint baseaddr, uint endaddr)
{
  struct fmap fm;
  fm.baseaddr = baseaddr;
  fm.size = endaddr - baseaddr;
  fm.f = f;
  f->ref++; //The fmap have a reference of the file, so increment the file refcount
  fm.present = 1;
  return fm;
}

// Duplicates a fmap
struct fmap
fmapdup(struct fmap fm)
{
  struct fmap fmdup;
  fmdup.baseaddr = fm.baseaddr;
  fmdup.size = fm.size;
  fmdup.f = fm.f;
  fmdup.f->ref++;
  fmdup.present = fm.present;
  return fmdup;
}
// Maps a file into memory
int 
mmap(int fd)
{
  struct proc *p = myproc();
  struct file *f = p->ofile[fd];
  if (f->type == FD_INODE && f->readable) {
    uint i = 0;
    while(i < NOFMAP && p->ofmap[i].present)
      i++;
  
    if(i == NOFMAP)
      return -1;

    
    struct stat fs;
    filestat(f, &fs);
    uint baseaddr = PGROUNDDOWN(p->sz) + PGSIZE;
    p->sz = baseaddr + fs.size;
    struct fmap *fm = &(p->ofmap[i]);
    *fm = createfmap(f, baseaddr, p->sz);

    // Reserve all the page, making possible
    // a efficient unmapping
    p->sz = PGROUNDDOWN(p->sz) + PGSIZE;    

    return baseaddr;

  }
  return -1;
}

// Updates the dirty pages to the mapped file
void static
syncfmap(struct fmap fm, struct proc *p)
{
  uint i = fm.baseaddr;
  uint endaddr = fm.baseaddr + fm.size;
  while(i < endaddr) {
    uint off = min(PGSIZE, (endaddr - fm.baseaddr)- PGROUNDDOWN(i));
   if(ispagewritable(p->pgdir, (char*) i)) {
      begin_op();
      ilock(fm.f->ip);
      writei(fm.f->ip, (char*) PGROUNDDOWN(i), i-fm.baseaddr, off);
      iunlock(fm.f->ip);
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

    if(p->ofmap[i].f->writable)
      syncfmap(p->ofmap[i], p);

    deallocuvm(p->pgdir, p->ofmap[i].baseaddr, p->ofmap[i].baseaddr + p->ofmap[i].size);  
    p->ofmap[i].present = 0;
    // Removes the file reference
    fileclose(p->ofmap[i].f);
    p->ofmap[i].f = 0;
    

    return 0;
  }
  return -1;
}











