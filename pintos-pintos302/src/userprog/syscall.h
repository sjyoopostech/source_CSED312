#ifndef USERPROG_SYSCALL_H
#define USERPROG_SYSCALL_H

void syscall_init(void);

struct lock *syscall_get_filesys_lock(void);

void syscall_exit(int);
void syscall_close(int);

struct vm_entry* check_vaddr(const void*);
struct vm_entry* check_vaddr_buf(const void* vaddr);

#endif /* userprog/syscall.h */
