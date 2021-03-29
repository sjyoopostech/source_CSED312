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

struct lock swap_lock;

struct block *swap_block;
struct bitmap *swap_bitmap;

void swap_init() {
    lock_init(&swap_lock);

    swap_block = block_get_role(BLOCK_SWAP);

    swap_bitmap = bitmap_create(block_size(swap_block) / (PGSIZE/BLOCK_SECTOR_SIZE));
    bitmap_set_all(swap_bitmap, true);

}

void in_swap(void *addr, size_t idx) {
    lock_acquire(&swap_lock);

    for (int block_cnt = 0; block_cnt < PGSIZE/BLOCK_SECTOR_SIZE; block_cnt++) {
        block_read(swap_block, idx*PGSIZE/BLOCK_SECTOR_SIZE + block_cnt, addr + block_cnt*BLOCK_SECTOR_SIZE);
    }
    bitmap_set(swap_bitmap, idx, true);

    lock_release(&swap_lock);

}

size_t out_swap(void *addr) {
    lock_acquire(&swap_lock);

    size_t idx = bitmap_scan(swap_bitmap, 0, 1, true);

    for (int block_cnt = 0; block_cnt < PGSIZE/BLOCK_SECTOR_SIZE; block_cnt++) {
        block_write(swap_block, idx*PGSIZE/BLOCK_SECTOR_SIZE + block_cnt, addr + block_cnt*BLOCK_SECTOR_SIZE);
    }
    bitmap_set(swap_bitmap, idx, false);

    lock_release(&swap_lock);

    return idx;
}

size_t free_swap(size_t idx) {
    lock_acquire(&swap_lock);

    bitmap_set(swap_bitmap, idx, true);

    lock_release(&swap_lock);
}