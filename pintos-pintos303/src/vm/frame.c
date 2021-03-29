#include "userprog/process.h"
#include <debug.h>
#include <inttypes.h>
#include <round.h>
#include <stdio.h>
#include <stdlib.h>

#include "filesys/directory.h"
#include "filesys/file.h"
#include "filesys/filesys.h"

#include "threads/flags.h"
#include "threads/init.h"
#include "threads/interrupt.h"
#include "threads/palloc.h"
#include "threads/thread.h"
#include "threads/vaddr.h"

#include "vm/page.h"
#include "vm/frame.h"
#include "vm/swap.h"

struct list lru_list;
struct list_elem *victim;
struct lock lru_lock;

void lru_init() {
    list_init(&lru_list);
    victim = NULL;
    lock_init(&lru_lock);
}

void add_lru_page(struct lru_page* p) {
    lock_acquire(&lru_lock);
    list_push_back(&lru_list, &p->lru_elem);
    lock_release(&lru_lock);
}

void del_lru_page(struct lru_page* p) {
    lock_acquire(&lru_lock);
    if (&p->lru_elem == victim) victim = list_next(victim);
    list_remove(&p->lru_elem);
    lock_release(&lru_lock);
}

struct lru_page* find_lru_page() {
    if (list_empty(&lru_list)) return NULL;

    struct lru_page* p = NULL;
    while (true) {
        if (victim == NULL || victim == list_end(&lru_list)) victim = list_begin(&lru_list);

        p = list_entry(victim, struct lru_page, lru_elem);
        victim = list_next(victim);
        if (pagedir_is_accessed(p->thread->pagedir, p->vm_entry->vaddr)) {
            pagedir_set_accessed(p->thread->pagedir, p->vm_entry->vaddr, false);
        }
        else break;
    }
    return p;
}

struct lru_page* alloc_page(enum palloc_flags flags) {

    struct lru_page* p = (struct lru_page*)malloc(sizeof(struct lru_page));
    p->thread = thread_current();
    p->paddr = palloc_get_page(flags);

    while (p->paddr == NULL) {
        struct lru_page* p_victim = find_lru_page();
        if (p_victim == NULL) return NULL;
    
        if (p_victim->vm_entry->type == VM_BIN) {
            if (pagedir_is_dirty(p_victim->thread->pagedir, p_victim->vm_entry->vaddr)) {
                p_victim->vm_entry->type = VM_ANON;
                p_victim->vm_entry->swap_slot = out_swap(p_victim->paddr);
            }
        }
        else if (p_victim->vm_entry->type == VM_FILE) {
            file_write_at(p_victim->vm_entry->file, p_victim->paddr, p_victim->vm_entry->read_bytes, p_victim->vm_entry->offset);
        }
        else if (p_victim->vm_entry->type == VM_ANON) {
            p_victim->vm_entry->swap_slot = out_swap(p_victim->paddr);
        }
        p_victim->vm_entry->is_loaded = false;
        free_page(p_victim->paddr);
        p->paddr = palloc_get_page(flags);
    };

    add_lru_page(p);

    return p;
}

void free_page(void *addr) {
    struct list_elem *e;

    for (e = list_begin(&lru_list); e != list_end(&lru_list); e = list_next(e)) {
        struct lru_page* p = list_entry(e, struct lru_page, lru_elem);
        if (p->paddr == addr) {
            pagedir_clear_page (p->thread->pagedir, p->vm_entry->vaddr);
            palloc_free_page(addr);
            del_lru_page(p);
            free(p);
            break;
        }
    }
}