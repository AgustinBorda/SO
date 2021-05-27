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
  
    
  for(n=0; n<N; n++){
    pid = fork();
    if (n == 3)
    	set_priority(3);
    if(pid < 0)
      break;    
    if(pid == 0)
      exit();
  }
  
}

int
main(void)
{
  forkagingtest();
  exit();
}
