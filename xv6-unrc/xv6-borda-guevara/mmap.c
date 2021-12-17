#include "types.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "x86.h"
#include "proc.h"
#include "mmap.h"
#include "spinlock.h"
#include "sleeplock.h"
#include "fs.h"
#include "file.h"
#include "stat.h"

struct filemap*
createfilemap(struct file *f, struct stat *fs, uint baseaddr, uint endaddr) {
  struct filemap *fm = 0;
  fm->baseaddr = baseaddr;
  fm->size = endaddr - baseaddr;
  fm->readable = f->readable;
  fm->writable = f->writable;
  fm->ip = f->ip;
  return fm;
}

int 
mmap(int fd) {
  struct proc *p = myproc();
  struct file *f = p->ofile[fd];
  if (f->type == FD_INODE && f->readable) {
    uint i = 0;

    while(i < NOMMAP && p->ommap[i])
      i++;
    
    if (i == NOMMAP)
      return -1;

   /* struct stat *fs = 0;
    filestat(f, fs);
    uint baseaddr = PGROUNDDOWN(p->sz);
    p->sz = PGROUNDDOWN(p->sz) + fs->size; 
    p->ommap[i] = createfilemap(f, fs, baseaddr, p->sz);
    return baseaddr;  */  
  }
  return -1;
}

