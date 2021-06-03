#include "types.h"
#include "stat.h"
#include "user.h"
int
main(void)
{
  int sd;
  int console_lock = semget(2,1);
  sd = semget(1,0);
  for (int i = 0; i < 10; i++) {
    semup(sd);
    semdown(console_lock);
    printf(0, "Yo, produci\n");
    semup(console_lock);
  }
  semclose(sd);
  exit();
}
