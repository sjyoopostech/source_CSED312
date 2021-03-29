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

#define F_STDIN 0
#define F_STDOUT 1
#define F_STDERR 2

void syscall_init (void);



#endif /* userprog/syscall.h */
