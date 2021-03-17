#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "proc.h"
#include "defs.h"
#include "x86.h"
#include "elf.h"
#include "ramfs.h"

int
exec(char *program)
{
    uint i, sz, sp;
    struct elfhdr *elf;
    struct proghdr *ph;
    pde_t *pgdir = 0, *oldpgdir;
    struct proc* curproc = myproc();
    uint off, ustack[1] = { 0xffffffff };
    struct file* f = get_file(program);
    
    cprintf("In exec system call: Loading program %s\n", program);

    if (f->type != FILE_EXEC)
        goto bad;

    elf = (struct elfhdr *) f->start;

    if (elf->magic != ELF_MAGIC)
        goto bad;
    
    // allocate and initialize a page directory
    if( (pgdir = setupkvm()) == 0)
        goto bad;

    // Load program into memory.
    sz = 0;
    for (i=0, off =elf->phoff; i < elf->phnum; i++, off+=sizeof(ph)) {
        ph = (struct proghdr*) ((char*)elf + off);
        if (ph->type != ELF_PROG_LOAD)
            continue;
        if( ph->memsz < ph->filesz)
            goto bad;
        if( ph->vaddr + ph->memsz < ph->vaddr)
            goto bad;
        // virtual address should be page aligned
        if (ph->vaddr % PGSIZE != 0)
            goto bad;
        // allocate memory for process
        if ((sz = allocuvm(pgdir, sz, ph->vaddr + ph->memsz)) == 0)
            goto bad;
        // copy from elf segment to process memory 
        if (copyout(pgdir, ph->vaddr, (char*)elf + ph->off, ph->filesz) < 0)
            goto bad;
        cprintf("Loaded program segment vaddr: %x, size: %d\n", 
                ph->vaddr, ph->filesz);
    }

    // Leave a page unallocated to detect stack overflow
    sz = PGROUNDUP(sz) + PGSIZE;

    // allocate a page for stack
    if((sz = allocuvm(pgdir, sz, sz + PGSIZE)) == 0)
        goto bad;

    // push fake return address from main()
    sp = sz - sizeof(ustack);
    if (copyout(pgdir, sp, ustack, sizeof(ustack)) < 0)
        goto bad;

    safestrcpy(curproc->name, f->name, sizeof(curproc->name));

    // Commit to the user image.
    oldpgdir = curproc->pgdir;
    curproc->pgdir = pgdir;
    curproc->sz = sz;
    curproc->tf->eip = elf->entry;  // main
    curproc->tf->esp = sp;
    switchuvm(curproc);
    freevm(oldpgdir);
    return 0;

bad:
    if (pgdir)
        freevm(pgdir);
    return -1;
}
