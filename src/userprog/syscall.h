#ifndef USERPROG_SYSCALL_H
#define USERPROG_SYSCALL_H
#include "threads/synch.h"

struct semaphore *filesys_sema;

void syscall_init (void);
void close_all_open_files(void);

#endif /* userprog/syscall.h */
