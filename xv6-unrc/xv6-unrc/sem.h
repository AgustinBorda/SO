struct sem {
  int value;          // the number of process that can access the critic region at the same time
  uint refcount;      // the number of process that references a semaphore
  spinlock lock;      // lock to ensure mutual exclusion
  int key;            // key (unique) to identify a semaphore
}


