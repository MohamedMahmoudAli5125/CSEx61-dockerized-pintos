#include "userprog/syscall.h"
#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include <syscall.h>
#include "threads/vaddr.h"
#include "userprog/pagedir.h"

static void syscall_handler(struct intr_frame *);
void check_valid_buffer (const void *buffer, unsigned size);
void check_valid_string(const void *str);
void check_valid_ptr(const void *vaddr);
void exit(int status);
void halt(void);
pid_t exec(const char *cmd_line);
int wait(pid_t pid);
bool create(const char *file, unsigned initial_size);
bool remove(const char *file);
int open(const char *file);
int filesize(int fd);
int read(int fd, void *buffer, unsigned size);
int write(int fd, const void *buffer, unsigned size);
void seek(int fd, unsigned position);
unsigned tell(int fd);
void close(int fd);

// Files are NOT thread safe in PintOs (2 threads could edit the same file at the same time).
// So we define this lock, so each time a thread wants to access a file it must first aquire the lock
static struct lock fs_lock;

void syscall_init(void)
{
  intr_register_int(0x30, 3, INTR_ON, syscall_handler, "syscall");

  // Initialize the files lock
  lock_init(&fs_lock);
}

static void
syscall_handler(struct intr_frame *f UNUSED)
{
  uint32_t *args = (uint32_t *)f->esp;
  check_valid_ptr(args);
  int sys_call_type = args[0];
  switch (sys_call_type)
  {
  case SYS_EXIT:
  {
    check_valid_buffer(args, 8);
    int status = (int)args[1];
    exit(status);
    break;
  }
  case SYS_HALT:
  {
    halt();
    break;
  }
  case SYS_EXEC:
  {
    check_valid_buffer(args, 8);
    char *cmd_line = (char *)args[1];
    check_valid_string(cmd_line);
    f->eax = exec(cmd_line);
    break;
  }
  case SYS_WAIT:
  {
    check_valid_buffer(args, 8);
    pid_t pid = (pid_t)args[1];
    f->eax = wait(pid);
    break;
  }
  case SYS_CREATE:
  {
    check_valid_buffer(args, 16);
    char *file = (char *)args[1];
    check_valid_string(file);
    unsigned initial_size = (unsigned)args[2];
    f->eax = create(file, initial_size);
    break;
  }
  case SYS_REMOVE:
  {
    check_valid_buffer(args, 8);
    char *file = (char *)args[1];
    check_valid_string(file);
    f->eax = remove(file);
    break;
  }
  case SYS_OPEN:
  {
    check_valid_buffer(args, 8);
    char *file = (char *)args[1];
    check_valid_string(file);
    f->eax = open(file);
    break;
  }
  case SYS_FILESIZE:
  {
    check_valid_buffer(args, 8);
    int fd = (int)args[1];
    f->eax = filesize(fd);
    break;
  }
  case SYS_READ:
  {
    check_valid_buffer(args, 16);
    int fd = (int)args[1];
    void *buffer = (void *)args[2];
    check_valid_ptr(buffer);
    unsigned size = (unsigned)args[3];
    check_valid_buffer(buffer, size);
    f->eax = read(fd, buffer, size);
    break;
  }
  case SYS_WRITE:
  {
    check_valid_buffer(args, 16);
    int fd = (int)args[1];
    void *buffer = (void *)args[2];
    check_valid_ptr(buffer);
    unsigned size = (unsigned)args[3];
    check_valid_buffer(buffer, size);
    f->eax = write(fd, buffer, size);
    break;
  }
  case SYS_SEEK:
  {
    check_valid_buffer(args, 12);
    int fd = (int)args[1];
    unsigned position = (unsigned)args[2];
    seek(fd, position);
    break;
  }
  case SYS_TELL:
  {
    check_valid_buffer(args, 8);
    int fd = (int)args[1];
    f->eax = tell(fd);
    break;
  }
  case SYS_CLOSE:
  {
    check_valid_buffer(args, 8);
    int fd = (int)args[1];
    close(fd);
    break;
  }
  default:
    exit(-1);
    break;
  }
}

void exit(int status)
{
  // TODO
}

void halt(void)
{
  shutdown_power_off();
}

pid_t exec(const char *cmd_line)
{
  // TODO
}

int wait(pid_t pid)
{
  // TODO
}

bool create(const char *file, unsigned initial_size)
{
  // TODO
}

bool remove(const char *file)
{
  // TODO
}

int open(const char *file)
{
  // TODO
}

int filesize(int fd)
{
  // TODO
}

int read(int fd, void *buffer, unsigned size)
{
  // TODO
}

int write(int fd, const void *buffer, unsigned size)
{
  // TODO
}

void seek(int fd, unsigned position)
{
  // TODO
}

unsigned tell(int fd)
{
  // TODO
}

void close(int fd)
{
  // TODO
}

void check_valid_buffer (const void *buffer, unsigned size) 
{
  if (size == 0)
  {
    return;
  }
  const char *current = (const char *) buffer;
  const char *end = current + size - 1;
  check_valid_ptr(current);
  // 1. Fast-forward our pointer to the start of the NEXT memory page
  current = (const char *) pg_round_down(current) + PGSIZE;
  // 2. Jump forward exactly one page at a time (4096 bytes) 
  // until we pass the end of the buffer
  while (current <= end) 
  {
    check_valid_ptr(current);
    current += PGSIZE;
  }

}
void check_valid_string(const void *str){
  check_valid_ptr(str);
  char *ptr = (char *) str;
  // While we haven't hit the end of the string...
  while (*ptr != '\0') 
  {
    ptr++; 
    // If the next character crosses a page boundary, validate the new page
    if ((uint32_t) ptr % PGSIZE == 0) {
      check_valid_ptr((const void *) ptr);
    }
  }
}

void check_valid_ptr(const void *vaddr){
  // 1. Check if null
  // 2. Check if it points to kernel memory (above PHYS_BASE)
  if (vaddr == NULL || !is_user_vaddr(vaddr))
  {
    exit(-1);
  }
  
  // 3. Check if the page is actually mapped in the page directory
  void *ptr = pagedir_get_page(thread_current()->pagedir, vaddr);
  if (ptr == NULL) {
    exit(-1);
  } 
}

