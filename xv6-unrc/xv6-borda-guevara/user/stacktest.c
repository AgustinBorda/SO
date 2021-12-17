#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"

int stacktest(int i) {
  if (i< 1000) {
  stacktest(i+1);
  }
  return 0;
}

int main() {
  printf(1, "stack test\n");
  stacktest(0);
  printf(1, "stack test OK\n");
  return 0;
}
