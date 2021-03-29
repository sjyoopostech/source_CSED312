#ifndef VM_PAGE_H
#define VM_PAGE_H

#include "lib/user/syscall.h"
#include "threads/synch.h"
#include "threads/thread.h"
#include "filesys/file.h"
#include <list.h>
#include <hash.h>


#define MAX_ARGS 128

#define VM_BIN 0 //load from binary
#define VM_FILE 1 //load from mapped file
#define VM_ANON 2 //load from swap partition

#define STACK_MAX (1024 * 1024)

/* A process control block. */
struct vm_entry{
	uint8_t type;
	void* vaddr;	
	bool writable;
	bool is_loaded;

	struct file* file;
	struct list_elem mmap_elem;

	size_t offset;
	size_t read_bytes;
	size_t zero_bytes;

	size_t swap_slot;

	struct hash_elem elem;
};

/* Project3 Edited - mmap */
/* Structure for memory mapped file */
struct file_mmap {
	mapid_t mapid;
	struct file* file;
	struct list_elem elem;
	struct list vm_entry_list;
};

void pt_init(struct hash *vmem);
static unsigned pt_hash_func(const struct hash_elem* e, void* aux);
static bool pt_less_func(const struct hash_elem* a, const struct hash_elem* b);

bool insert_vm_entry(struct hash* vm_hash, struct vm_entry* vmem);
bool delete_vm_entry(struct hash* vm_hash, struct vm_entry* vmem);

struct vm_entry* alloc_page(void* addr, bool writable);

struct vm_entry* find_entry(void *vaddr);
void vm_remove_all(struct hash *vm);
static void vm_remove_page(struct hash_elem* page, void* aux);


void check_buffer(void* buf, unsigned size, void* esp, bool write);
void check_string(const void* str, void* esp);

bool load_entry(struct vm_entry* e,void * buffer);
bool load_mmap_file (struct vm_entry *e, void *buffer);

#endif /* vm/page.h */
