#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include <error-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "../devices/shutdown.h"
//#include "../lib/user/syscall.h"
#include "process.h"
#include "pagedir.h"
#include "threads/vaddr.h"
#include "threads/synch.h"

/* Process identifier. */
typedef int pid_t;

static void syscall_handler (struct intr_frame *);
void halt (void);
void exit (int status, struct intr_frame *f UNUSED);
pid_t exec (const char *file);
int wait (pid_t pid);
bool create (const char *file, unsigned initial_size);
bool remove (const char *file);
int open (const char *file);
int filesize (int fd);
int read (int *fd_, void *buffer, unsigned *size_);
int write (int *fd_, const void *buffer, unsigned *size_);
void seek (int fd, unsigned position);
unsigned tell (int fd);
void close (int fd);

bool is_valid_pointer(const void *usr_ptr);
bool is_valid_buffer(char *buffer, uint32_t size);



void
syscall_init (void) 
{
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
}

static void
syscall_handler (struct intr_frame *f UNUSED) 
{

  if(!is_valid_pointer(f->esp)) exit(PTR_FAILURE, f);
  int *syscall_num = f->esp;
  

  /*printf("stack for eax, %p\n",f->eax);
  hex_dump((uintptr_t) (f->eax), (void*)(f->eax ), (size_t) 52, true);
  printf("\n");

  printf("stack for esp, %p\n", f->esp);
  hex_dump((uintptr_t) (f->esp ), (void*)(f->esp ), (size_t) 52, true);
  printf("\n");*/

  f->esp += sizeof(void*);


  if(*syscall_num == SYS_HALT){
    //printf("selected system call %d, HALT\n",*syscall_num);
    halt();
  }else if(*syscall_num == SYS_EXIT){
    //printf("selected system call %d, EXIT\n",*syscall_num);

    int *status = f->esp;
    exit(*status, f);
    
  }else if(*syscall_num == SYS_EXEC){
    //printf("selected system call %d, EXEC\n",*syscall_num);
  }else if(*syscall_num == SYS_WAIT){
    //printf("selected system call %d, WAIT\n",*syscall_num);
  }else if(*syscall_num == SYS_CREATE){
    //printf("selected system call %d, CREATE\n",*syscall_num);
  }else if(*syscall_num == SYS_REMOVE){
    //printf("selected system call %d, REMOVE\n",*syscall_num);
  }else if(*syscall_num == SYS_OPEN){
    //printf("selected system call %d, OPEN\n",*syscall_num);
  }else if(*syscall_num == SYS_FILESIZE){
    //printf("selected system call %d, FILEZISE\n",*syscall_num);
  }else if(*syscall_num == SYS_READ){
    //printf("selected system call %d, READ\n",*syscall_num);
  }else if(*syscall_num == SYS_WRITE){
    //printf("selected system call %d, WRITE\n",*syscall_num);
    
    int *fd= f->esp;
    f->esp += sizeof(void*);
    char **buffer= f->esp;
    f->esp += sizeof(void*); 
    uint32_t *size= f->esp;
    
    int bytes_written = 0;
    if(is_valid_buffer(*buffer, *size)){
       bytes_written = write(fd, *buffer, size);
    } else {
      exit(EXIT_FAILURE, f);
    }

    f->eax = bytes_written;

    f->esp -= sizeof(void*) *2;


  }else if(*syscall_num == SYS_SEEK){
    //printf("selected system call %d, SEEK\n",*syscall_num);
  }else if(*syscall_num == SYS_TELL){
    //printf("selected system call %d, TELL\n",*syscall_num);
  }else if(*syscall_num == SYS_CLOSE){
    //printf("selected system call %d, CLOSE\n",*syscall_num);
  } else {
    //invalid system call number
    //printf("invalid system call # %d", *syscall_num);
    exit(UNKNOWN_FAILURE,f);
  }

  // ebx is where you wnat t get the stuff from the user
  //putbuf();
  //file_write();
  f->esp -= sizeof(void*);

  

  /*printf("stack for eax, %p\n",f->eax);
  hex_dump((uintptr_t) (f->eax), (void*)(f->eax ), (size_t) 52, true);
  printf("\n");*/

  

      
     //int fd=*(p+1);
     //char *buffer=*(p+2); 
     //unsigned size=*(p+3);
     //first 4 bytes is the argument number 
     //second 4 bytes is the file descriptor 1 = write
     //third 4 bytes is the buffer
     //fouth 4 bytes is the size

    //exit(0);

  //intr_dump_frame(f);
  //thread_exit ();
}

void halt (void) {
  shutdown_power_off();
}

void exit(int status, struct intr_frame *f UNUSED) {
  
  //printf("status %d\n", status);

  if(thread_current()->pagedir == NULL) status = EXIT_FAILURE;

  thread_current()->exit_status = status;
  f->eax = status;
  
  lock_acquire(proc_cond_lock);
  cond_signal(process_cond, proc_cond_lock);
  lock_release(proc_cond_lock);
  //printf("tid %d\n", thread_current()->tid);
  thread_exit();
  
}

pid_t exec (const char *file){

}

int wait (pid_t pid){
  return process_wait(pid);
}

bool create (const char *file, unsigned initial_size){
  
  return false;
}

bool remove (const char *file){
  
  return false;
}

int open (const char *file){ 
  
  return 0;
}

int filesize (int fd) {
  
  return 0;
}

int read (int *fd, void *buffer, unsigned *size){
  
  return 0;
}

int write (int *fd_, const void *buffer, unsigned *size_){
  int fd = (*fd_);
  unsigned size = (*size_);
  
  //printf("file descriptor %d\n", (*fd_));
  
  
  //printf("the buffer is: '%s'\n", buffer);
   
  
  //printf("the size is %d\n", (*size_));
  
  int bytes_written =0;
  if(fd ==1){
    int size_buf = size;
    for(; size_buf>= 300; size_buf -= 300){
      putbuf(buffer, size_buf);
      bytes_written += 300;
    }
    if(size_buf > 0) {
      putbuf(buffer, size_buf);
      bytes_written += size_buf;
    }

  } else if(fd ==0){
    
  }

  return bytes_written;
  
}

/*void seek (int fd, unsigned position) {
  
}

unsigned tell (int fd) {
   
   return 0;
}

void close (int fd){

}*/

bool is_valid_pointer(const void *usr_ptr){
  if( !(is_user_vaddr(usr_ptr) || usr_ptr < ((void *) 0x08048000) ) ) return false;
  return true;
}

bool is_valid_buffer(char *buffer, uint32_t size){
  char *b = buffer;
  uint32_t i =0;
  for(; i<size; i++)
    if(!is_valid_pointer( &b[i])) return false;
  return true;
}
