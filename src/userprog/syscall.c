#include "userprog/syscall.h"
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
  printf ("system call!\n");
  printf("vec # = %x\n",f->vec_no);

  printf("eax = %p\n",f->eax);
  printf("stack for eax\n");
  hex_dump((uintptr_t) (f->eax), (void*)(f->eax-32 ), (size_t) 52, true);
  printf("\n");

  printf("ebx = %p\n",f->ebx);
  printf("stack for ebx\n");
  hex_dump((uintptr_t) (f->ebx ), (void*)(f->ebx-32), (size_t) 52, true);
  printf("\n");

  /*printf("ecx = %p\n",f->ecx);
  printf("stack for ecx\n");
  hex_dump((uintptr_t) (f->ecx +32), (void*)(f->ecx +32), (size_t) 52, true);
  printf("\n");

  printf("edx = %p\n",f->edx);
  printf("stack for edx\n");
  hex_dump((uintptr_t) (f->edx +32), (void*)(f->edx +32), (size_t) 52, true);
  printf("\n");

  printf("esi = %p\n",f->esi);
  printf("stack for esi\n");
  hex_dump((uintptr_t) (f->esi +32), (void*)(f->esi +32), (size_t) 52, true);
  printf("\n");*/

  printf("edi = %p\n",f->edi);
  printf("stack for edi\n");
  hex_dump((uintptr_t) (f->edi ), (void*)(f->edi -32), (size_t) 52, true);
  printf("\n");

  printf("esp = %p\n",f->esp);
  int *p = f->esp;
  printf("write syscall num %d\n", SYS_WRITE);
  printf("test num %d\n", *p);
  printf("stack for esp\n");
  hex_dump((uintptr_t) (f->esp ), (void*)(f->esp ), (size_t) 52, true);
  printf("\n");

  printf("ebp = %p\n",f->ebp);
  printf("stack for ebp\n");
  hex_dump((uintptr_t) (f->ebp ), (void*)(f->ebp +32), (size_t) 40, true);
  printf("\n");

  f->ebp += sizeof(void*);
  void *return_addr;
  memcpy(return_addr, f->ebp, sizeof(void*));
  printf("return address %p\n", return_addr);

  f->ebp += sizeof(int);
  int argc;
  memcpy(&argc, f->ebp, sizeof(int));
  printf("argc %d\n", argc);

  /*f->ebp += sizeof(void*);
  char **argv;
  memcpy(&argv, f->ebp, sizeof(void*));
  printf("argv %p\n", argv);
  for(int i =0; i<argc; i++) printf("argv[%d] '%s'\n",i, argv[i]);*/


  intr_dump_frame(f);
  thread_exit ();
}
