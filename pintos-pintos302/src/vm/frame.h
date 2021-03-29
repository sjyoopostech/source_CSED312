#ifndef VM_FRAME_H
#define VM_FRAME_H

#include "lib/user/syscall.h"
#include "threads/synch.h"
#include "threads/thread.h"
#include "filesys/file.h"
#include <list.h>
#include <hash.h>

void lru_init();
void add_lru_page(struct lru_page* p);
void del_lru_page(struct lru_page* p);
struct lru_page* find_lru_page();

struct lru_page* alloc_page(enum palloc_flags flags);
void free_page(void *addr);

#endif /* vm/frame.h */
