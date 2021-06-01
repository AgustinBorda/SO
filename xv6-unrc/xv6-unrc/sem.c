void
switchkvm(void);

int
copyout(pde_t* pgdir);

void
clearpteu(pde_t* pgdir, char* uva);

// number of elements in fixed-size array
#define NELEM(x) (sizeof(x)/sizeof((x)[0]))



int
semget(int key, int init_value);


int
semdown(int semid);


int
semclose(int semid);


int
semup(int semid);


int
semclose(int semid);
