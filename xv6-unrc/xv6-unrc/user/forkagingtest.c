// Test that fork fails gracefully.
// Tiny executable so that the limit can be filling the proc table.

#include "types.h"
#include "stat.h"
#include "user.h"

#define N  1000



void
forkagingtest(void)
{
  int n, pid;
  
    
  printf(1, "fork aging test\n");
  n = fork();
  if (n) {
    pid = fork();
    if (pid) {
      set_priority(3);
      sleep(10);
      set_priority(3);
      printf(1, "fork aging test OK\n");
      kill(n);
      kill(pid);
      wait();
      wait();
      exit();
    }
    else {
      set_priority(0);
      for (;;) {
        set_priority(0);
      }
    }
  }
  else {
    set_priority(0);
    for (;;) {
      set_priority(0);
    }
  } 
}

int
main(void)
{
  forkagingtest();
  exit();
}
