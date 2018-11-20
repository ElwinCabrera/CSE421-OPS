#include "userprog/syscall.h"
#include "../lib/user/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"

static void syscall_handler (struct intr_frame *);

void
syscall_init (void) 
{
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
}

static void
syscall_handler (struct intr_frame *f UNUSED) 
{

  // ebx is where you wnat t get the stuff from the user
  //putbuf();
  //file_write();
  
  printf("system call!\n");

 /* printf("eax = %p\n",f->eax);
  printf("stack for eax\n");
  hex_dump((uintptr_t) (f->eax), (void*)(f->eax-32 ), (size_t) 52, true);
  printf("\n");*/

  int *sys_call_num = f->esp;
  printf("esp = %p\n",f->esp);
  printf("write systcall num %d\n", SYS_WRITE);
  printf("System call num requested %d\n", *sys_call_num);
  printf("stack for esp\n");
  hex_dump((uintptr_t) (f->esp ), (void*)(f->esp ), (size_t) 52, true);
  printf("\n");

  //exit(0);



  intr_dump_frame(f);
  thread_exit ();
}
