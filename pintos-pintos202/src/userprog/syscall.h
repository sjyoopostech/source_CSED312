#ifndef USERPROG_SYSCALL_H
#define USERPROG_SYSCALL_H

#include <stdio.h>
#include <syscall-nr.h>
#include "threads/interrupt.h"
#include "threads/thread.h"
#include "filesys/filesys.h"
#include "threads/synch.h"
#include "devices/shutdown.h"
#include "threads/palloc.h"

void syscall_init (void);

void halt();
void exit(int status);
tid_t exec(const char* cmdline);
int wait(tid_t pid);
bool create(const char* file, unsigned file_size);
bool remove(const char* file);
void patch_argument(void* esp, int* arg ,int count);
void check_address_valid(void* addr);
int read(int fd, void* buffer, unsigned size);
int write(int fd, const void* buffer, unsigned size);

#endif /* userprog/syscall.h */
