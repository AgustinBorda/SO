#include "types.h"
#include "stat.h"
#include "user.h"
int
main(void)
{
  int sd;
  int console_lock;
  sd = semget(1, 0);
  console_lock = semget(2,1);
  for (int i = 0; i < 10; i++) {
    semdown(sd);
    semdown(console_lock);
    printf(0, "Yo, consumi\n");
    semup(console_lock);
  }
  semclose(sd);
  exit();
}
