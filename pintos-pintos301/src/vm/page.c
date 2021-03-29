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
#include "userprog/syscall.h"

void vmem_init(struct hash* vmem) {
	hash_init(vmem, pt_hash_func,pt_less_func,NULL);
}

static unsigned pt_hash_func(const struct hash_elem* e, void* aux) {
	const struct vm_entry* vmem;
	vmem = hash_entry(e,struct vm_entry, elem);
	return hash_int(vmem->vaddr);
}

static bool pt_less_func(const struct hash_elem* a, const struct hash_elem* b) {
	const struct vm_entry* vm_a;
	const struct vm_entry* vm_b;

	vm_a = hash_entry(a, struct vm_entry, elem);
	vm_b = hash_entry(b, struct vm_entry, elem);

	if (vm_a->vaddr < vm_b->vaddr) {
		return true;
	}
	return false;
}

bool insert_vm_entry(struct hash* vm_hash, struct vm_entry* vmem) {
	struct hash_elem* e;
	if (vmem == NULL) {
		return false;
	}
	else {
		e = hash_insert(vm_hash, &vmem->elem);
		if (e != NULL) {
			return false;
		}
		else {
			return true;
		}
	}
}

bool delete_vm_entry(struct hash* vm_hash, struct vm_entry* vmem) {
	struct hash_elem* e;
	if (vmem == NULL) {
		return false;
	}
	else {
		e = hash_delete(vm_hash, &vmem->elem);
		if (e == NULL) {
			return false;
		}
		else {
			return true;
		}
	}
}

struct vm_entry* alloc_page(void* addr, bool writable) {
	struct thread* cur = thread_current();	
	struct vm_entry* e = malloc(sizeof (struct vm_entry));

	if (e != NULL) {
		//initialize allocated page
		uint8_t type;
		e->vaddr = addr;
		e->writable = writable;
		e->is_loaded = false;

		e->type = VM_BIN;

		e->file = NULL;
	
		e->offset=0;
		e->read_bytes=0;
		e->zero_bytes=0;
		e->swap_slot=0;

		if (insert_vm_entry(&cur->hash_table, e)!=NULL){
			return e;
		}else{
			return NULL;
		}
	}
}

struct vm_entry* find_entry(void* vaddr) {
	struct vm_entry page;
	page.vaddr = pg_round_down(vaddr);
	struct hash_elem* found = hash_find(&thread_current()->hash_table, &(page.elem));
	if (found == NULL) {
		//for stack allocation
		// if ((page.vaddr > PHYS_BASE -STACK_MAX)&&((void*)thread_current()->stack -32 <vaddr)) {
		// 	return alloc_page(page.vaddr,true);
		// }
		return NULL;
	}
	return hash_entry(found,struct vm_entry, elem);
}

void vm_remove_all(struct hash* vm) {
	if (vm != NULL) {
		hash_destroy(vm, vm_remove_page);
	}
}

static void vm_remove_page(struct hash_elem * page,void *aux) {
	struct vm_entry* e = hash_entry(page, struct vm_entry, elem);
	//TODO with frame
	hash_delete(&thread_current()->hash_table,page);
	pagedir_clear_page(thread_current()->pagedir, (void*)e->vaddr);
	free(e);
}

void check_buffer(void* buf, unsigned size, void* esp, bool write) {
	struct vm_entry* e1 = check_vaddr_buf(buf);
	struct vm_entry* e2 = check_vaddr_buf(buf+size);

	if (e1==NULL||e2==NULL) {
		syscall_exit(-1);
	}
	void* buf_page;
	for (buf_page = pg_round_down(buf); buf_page <= buf + size; buf_page += PGSIZE) {
		struct vm_entry* e= check_vaddr_buf(buf_page);
		if (write && !e->writable) {
			syscall_exit(-1);
		}
	}
}

void check_string(const void* str, void* esp) {
	struct vm_entry* e = check_vaddr_buf(str);
	if (e == NULL) {
		syscall_exit(-1);
	}
}

bool load_entry(struct vm_entry* e,void * buffer) {

	bool success = false;
	
	if (e->read_bytes != 0) {
		//load using file_read_at
		if (file_read_at(e->file, buffer, e->read_bytes, e->offset)<=0) {
			palloc_free_page(buffer);
			return false;
		}
		memset(buffer + e->read_bytes, 0, e->zero_bytes);
	}
	return true;
}

/* Project3 Edited - mmap */
bool load_mmap_file (struct vm_entry *e, void *buffer) {

	bool success = false;
	
	if (file_read_at(e->file, buffer, e->read_bytes, e->offset) != e->read_bytes) {
		palloc_free_page(buffer);
		return false;
	}
	memset(buffer + e->read_bytes, 0, e->zero_bytes);
	return true;
}