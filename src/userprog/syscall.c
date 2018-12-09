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
#include "threads/malloc.h"
#include "filesys/file.h"
#include "filesys/filesys.h"
#include "devices/input.h"


/* Process identifier. */
typedef int pid_t;

struct semaphore filesys_sema;



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

void* is_valid_pointer(const void *usr_ptr);
bool is_valid_buffer(char *buffer, uint32_t size);
struct thread_file* find_file_from_opend(struct list *open_files, int fd);



void
syscall_init (void) 
{
  if(!(proc_cond_lock && process_cond)){
    proc_cond_lock = malloc(sizeof(struct lock));
    process_cond = malloc(sizeof(struct condition));
    lock_init(proc_cond_lock);
    cond_init(process_cond);
  }
  sema_init(&filesys_sema,1);
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
}

static void
syscall_handler (struct intr_frame *f UNUSED) 
{
  
  unsigned *esp = f->esp;
  is_valid_pointer(esp);

  if(*esp == SYS_HALT){
    //printf("selected system call %d, HALT\n",*esp);
    halt();
  }else if(*esp == SYS_EXIT){
    //printf("selected system call %d, EXIT\n",*esp);

    int *status = esp+1;
    exit(*status, f);
    
  }else if(*esp == SYS_EXEC){
    //printf("selected system call %d, EXEC\n",*esp);
    
  }else if(*esp == SYS_WAIT){
    //printf("selected system call %d, WAIT\n",*esp);
    tid_t *tid = esp+1;
    //printf("Thread id to wait %d\n", *tid);
    f->eax = wait(*tid);
  }else if(*esp == SYS_CREATE){
    //printf("selected system call %d, CREATE\n",*esp);
    char **file = is_valid_pointer(esp+4);
    uint8_t *initial_size = is_valid_pointer(esp+5);
    //printf("file: %s, size: %d\n",*file, *initial_size);
    f->eax = create(*file, *initial_size);
  }else if(*esp == SYS_REMOVE){
    //printf("selected system call %d, REMOVE\n",*esp);
    const char **file= esp+1;
    //printf("file %s \n",*file);
    f->eax = remove(*file);

  }else if(*esp == SYS_OPEN){
    //printf("selected system call %d, OPEN\n",*esp);
    int *fd = esp+1;
    f->eax = open(*fd);
  }else if(*esp == SYS_FILESIZE){
    //printf("selected system call %d, FILEZISE\n",*esp);
    int *fd = esp+1;
    f->eax = filesize(*fd);
  }else if(*esp == SYS_READ){
    //printf("selected system call %d, READ\n",*esp);
    int *fd= esp+1;
    char **buffer= esp+2; 
    uint32_t *size= esp+3;
    
    if(is_valid_buffer(*buffer, *size)) f->eax = read(fd, *buffer, size);
     
  }else if(*esp == SYS_WRITE){
    //printf("selected system call %d, WRITE\n",*esp);
    
    int *fd= esp+1;
    char **buffer= esp+2;
    uint32_t *size= esp+3;
    
    if(is_valid_buffer(*buffer, *size)) f->eax = write(fd, *buffer, size);
    

  }else if(*esp == SYS_SEEK){
    //printf("selected system call %d, SEEK\n",*esp);
    int *fd = esp+1;
    unsigned *pos = esp+2;
    seek(*fd, *pos);
  }else if(*esp == SYS_TELL){
    //printf("selected system call %d, TELL\n",*esp);
    int *fd = esp+1;
    f->eax = tell(*fd);
  }else if(*esp == SYS_CLOSE){
    //printf("selected system call %d, CLOSE\n",*esp);
    tid_t *fd = esp+1;
    //printf("tid %d\n", *fd);
    close(*fd);
  } else {
    //invalid system call number
    //printf("invalid system call # %d", *esp);
    exit(UNKNOWN_FAILURE,f);
  }
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

  return 0;
}

int wait (pid_t pid){
  return process_wait(pid);
}

bool create (const char *file, unsigned initial_size){
  sema_down(&filesys_sema);
  bool success = filesys_create(file, initial_size);
  sema_up(&filesys_sema);
  return success;

}

bool remove (const char *file){
  sema_down(&filesys_sema);
  bool success = filesys_remove(file);
  sema_up(&filesys_sema);
  return success;
}

int open (const char *file){ 
  sema_down(&filesys_sema);
  struct file *f = filesys_open(file);
  sema_up(&filesys_sema);
  if(f){
    struct thread *t = thread_current();
    struct thread_file *tf = malloc(sizeof(struct thread_file));
    tf->file = f;
    tf->fd = t->last_fd;
    list_push_back(&t->open_files, &tf->thread_file_elem);
    t->last_fd += 1;
    return tf->fd;
  }
  return -1;
}

int filesize (int fd) {
  int size= 0;
  struct thread *t = thread_current();
  struct thread_file *tf = find_file_from_opend(&t->open_files, fd);
  sema_down(&filesys_sema);
  size = file_length(tf->file);
  sema_up(&filesys_sema);
  return size;
}

int read (int *fd, void *buffer, unsigned *size){
  int bytes_read=0;
  if(fd ==0){
    int i;
    char *buffer_ = buffer;
    for(i=0; i<size; i++){
      buffer_[i] = input_getc();
    }
    bytes_read += size;
  } else {
    struct thread *t = thread_current();
    struct thread_file *tf = find_file_from_opend(&t->open_files, fd);
    if(tf){
      sema_down(&filesys_sema);
      bytes_read = file_write(tf->file, buffer, (off_t)size);
      sema_up(&filesys_sema);
    }

  }
  
  return bytes_read;
}

int write (int *fd_, const void *buffer, unsigned *size_){
  int fd = (*fd_);
  unsigned size = (*size_);
  
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

  } else {
    struct thread *t = thread_current();
    struct thread_file *tf = find_file_from_opend(&t->open_files, fd);
    if(tf){
      sema_down(&filesys_sema);
      bytes_written = file_write(tf->file, buffer, (off_t)size);
      sema_up(&filesys_sema);
    }

  }

  return bytes_written;
  
}

void seek (int fd, unsigned position) {
  struct thread *t = thread_current();
  struct thread_file *tf = find_file_from_opend(&t->open_files, fd);
  sema_down(&filesys_sema);
  file_seek(tf->file, position);
  sema_up(&filesys_sema);
}

unsigned tell (int fd) {
  int curr_pos= 0;
  struct thread *t = thread_current();
  struct thread_file *tf = find_file_from_opend(&t->open_files, fd);
  sema_down(&filesys_sema);
  curr_pos = file_tell(tf->file);
  sema_up(&filesys_sema);
  return curr_pos;
}

void close (int fd){
  struct thread *t = thread_current();
  struct thread_file *tf = find_file_from_opend(&t->open_files, fd);
  sema_down(&filesys_sema);
  file_close(tf->file);
  sema_up(&filesys_sema);
}

void* is_valid_pointer(const void *usr_ptr){
  if( !(is_user_vaddr(usr_ptr) || usr_ptr < ((void *) 0x08048000) ) ) exit(PTR_FAILURE,NULL);
  void *kaddr = pagedir_get_page(thread_current()->pagedir, usr_ptr);
  if(!kaddr) exit(PTR_FAILURE,NULL);
  return kaddr;
}

bool is_valid_buffer(char *buffer, uint32_t size){
  char *b = buffer;
  uint32_t i =0;
  for(; i<size; i++)
    if(!(is_user_vaddr(&b[i]) || &b[i] < ((void *) 0x08048000) )) return false;
  return true;
}

struct thread_file* find_file_from_opend(struct list *open_files, int fd){
  struct list_elem *e;
      for (e = list_begin (open_files); e != list_end (open_files); e = list_next (e)){
          struct thread_file *tf = list_entry (e, struct thread_file, thread_file_elem);
          if(tf->fd == fd) return tf;
        }
   return NULL;
}