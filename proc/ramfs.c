/*=============================================================================
 * In memory filesystem.
 * User programs are linked as binary objects at end of kernel image.
 * For each user program p, the linker inserts symbols _binary_p_start,
 * binary_p_end and _binary_p_size.
 * ==========================================================================*/

#include "types.h"
#include "ramfs.h"
#include "defs.h"

extern char _binary_userinit_start[], _binary_userinit_size[];

// root directory
static struct file root[] = {
  { .name = "/init",
    .type = FILE_EXEC,
    .start = _binary_userinit_start,
    .size = _binary_userinit_size
  },
  {.name = 0, .type = FILE_NONE, .start = 0, .size = 0}
};

struct file * get_file(char *name)
{
    struct file * f = root;

    while (f->name && strncmp(name, f->name, 16) != 0)
        f++;
    return f; 
}
