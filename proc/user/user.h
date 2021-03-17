#include "../types.h"
#include "../x86.h"

// system calls
int fork(void);
int exit(void) __attribute__((noreturn));
int wait(void);
int kill(int);
int exec(char*, char**);
int getpid(void);
char* sbrk(int);
int sleep(int);
int uptime(void);
int setpriority(int);

// ulib.c
char* strcpy(char*, const char*);
void* memmove(void*, const void*, int);
char* strchr(const char*, char c);
int   strcmp(const char*, const char*);
void  print(const char*);
uint  strlen(const char*);
void* memset(void*, int, uint);
int   atoi(const char*);
void  reverse(char*, int);
char* itoa(int, char*, int);
