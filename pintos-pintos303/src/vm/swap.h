#ifndef VM_SWAP_H
#define VM_SWAP_H

#include "lib/user/syscall.h"
#include "threads/synch.h"
#include "threads/thread.h"
#include "filesys/file.h"
#include <list.h>
#include <hash.h>

void swap_init();
void in_swap(void *addr, size_t idx);
size_t out_swap(void *addr);

#endif /* vm/swap.h */
