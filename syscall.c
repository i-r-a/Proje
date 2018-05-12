#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "devices/shutdown.h"
#include "filesys/filesys.h"
#include "filesys/file.h"
#include "threads/malloc.h"
typedef int pid_t;
static void syscall_handler (struct intr_frame *);
void syscall_halt (void);
int syscall_wait(pid_t aa);
void syscall_exit(int a);
bool syscall_remove(const char *name, struct intr_frame *);
pid_t syscall_exec(const char* cmd_line);
int syscall_open(const char* file);
int syscall_filesize(int fd);
int syscall_read(int fd, void* buffer, unsigned size);
int syscall_write(int fd, const void* buffer, unsigned size);
void syscall_seek(int fd, unsigned position);
unsigned syscall_tell(int fd);
void syscall_close(int fd);
bool check(const void *addr);
struct lock files;


void
syscall_init (void) 
{
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
}

static void
syscall_handler (struct intr_frame *f) 
{
 // printf ("system call!\n");
 // thread_exit (0);
 if(!(check(f->esp)))
 {
 		
 		quit();
 }
  switch(*(int*) f->esp){
  case SYS_HALT:
  	syscall_halt();
  	break;
  case SYS_WAIT:
  	//pid_t pid;
  	//pid_t pid = (*int*) f->esp+1;
  	//f->eax = syscall_wait(pid);
  	break;
  case SYS_CREATE:
  	bool check1 = check(f->esp+1);
  	bool check2 = check(*(f->esp+1));
  	if(check(f->esp+1)||check(*(f->esp+1)))
  	{
  		quit();
  	}
  	else
  	{
  		char *name = (const char *)*(unsigned int *)f->esp+1;
  		unsigned int size = *((unsigned int *)f->esp+2);
  		lock_acquire(&files);
  		filesys_create(name, size);
  		lock_release(&files);
  	}
  	break;
  case SYS_REMOVE:
  	//check validity
  	syscall_remove(*(f->esp+1), f);
  	break;
  case SYS_OPEN:		//const char* file
  	//check validity
  	syscall_open(*(f->esp+1));
  	break;
	}
}
bool
check(const void* addr)
{
	if(!is_use_vaddr(addr)){
	
	}
	quit();
}
void 
quit()
{

}
void
syscall_halt(void)
{
	shutdown_power_off();
}
int
syscall_wait(pid_t aa)
{
	return process_wait(aa);
}
void
syscall_exit(int a)
{
	//thread_current->exit_status = a;
	thread_exit(a);
}

bool
syscall_remove(const char *name, struct intr_frame *f)
{

//lock
	f->eax = filesys_remove(name);
	bool retVal = false;
	if(f->eax != NULL)
	{
		retVal = true;
	}
//release lock
	return retVal;	
}
int
syscall_open(const char *file)
{
	//acquire lock
	struct file *opened = filesys_open(file);
	if(!opened){
	//release lock
	return -1;
	}
	//release lock
	int retVal = process_add_file(opened);
	return retVal;
}

