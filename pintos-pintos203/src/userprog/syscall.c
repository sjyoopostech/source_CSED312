
#include "userprog/syscall.h"
#include "threads/vaddr.h"

static void syscall_handler (struct intr_frame *);

void patch_argument(void* esp, int* arg, int count);
void check_address_valid(void* addr);

void halt();
void exit(int status);
tid_t exec(const char* cmdline);
int wait(tid_t pid);

bool create(const char* file, unsigned file_size);
bool remove(const char* file);
int open(const char* file);
int filesize(int fd);
int read(int fd, void* buffer, unsigned size);
int write(int fd, const void* buffer, unsigned size);
void seek(int fd, unsigned position);
unsigned tell(int fd);
void close(int fd);

void syscall_init (void)
{
	lock_init(&file_lock);
  intr_register_int (0x30, 3, INTR_ON, syscall_handler, "syscall");
}

static void
syscall_handler (struct intr_frame *f UNUSED)
{
	/* Project2 Edited */
	/* get stack pointer from inturrupt frame and run system call using first argument of stack(system call number) */

	int* args;  //memory allocation
	int sysnum;

	void* stack_pt = f->esp;
	args= palloc_get_page(0);
	////////////////////////////
	check_address_valid(stack_pt);

	patch_argument(stack_pt, args, 1);

	sysnum = args[0];

	stack_pt = (int*)stack_pt + 1;

	switch (sysnum){
	case SYS_HALT:
		palloc_free_page(args);
		halt();
		break;
	case SYS_EXIT:
		check_address_valid(stack_pt);
		patch_argument(stack_pt, args, 1);
		
		int a00 = args[0];
		palloc_free_page(args);

		exit(a00);
		break;
	case SYS_EXEC:
		patch_argument(stack_pt, args, 1);
		
		check_address_valid(stack_pt);

		const char* a10 = args[0];
		palloc_free_page(args);
		lock_acquire(&file_lock);
		f->eax = exec(a10);
		lock_release(&file_lock);
		break;
	case SYS_WAIT:
		patch_argument(stack_pt, args, 1);
		check_address_valid(stack_pt);

		tid_t a20 = args[0];
		palloc_free_page(args);
		f->eax = wait(a20);
		break;

	case SYS_CREATE:
		check_address_valid((char*)stack_pt);
		check_address_valid((char*)stack_pt + 4);
		patch_argument(stack_pt, args, 2);

		check_address_valid((const char*)args[0]);

		const char* a30 = args[0];
		unsigned a31 = args[1];
		palloc_free_page(args);

		lock_acquire(&file_lock);
		f->eax = create(a30, a31);
		lock_release(&file_lock);
		break;

	case SYS_REMOVE:
		check_address_valid((char*)stack_pt);
		patch_argument(stack_pt, args, 1);
		
		check_address_valid((const char*)args[0]);

		const char* a40 = args[0];
		palloc_free_page(args);

		lock_acquire(&file_lock);
		f->eax = remove(a40);
		lock_release(&file_lock);
		break;

	case SYS_OPEN:
		check_address_valid((char*)stack_pt);
		patch_argument(stack_pt, args, 1);

		check_address_valid((struct file*)args[0]);

		struct file* a50 = args[0];
		palloc_free_page(args);

		lock_acquire(&file_lock);
		f->eax = open(a50);
		lock_release(&file_lock);
		break;

	case SYS_FILESIZE:
		check_address_valid((char*)stack_pt);
		patch_argument(stack_pt, args, 1);

		int a60 = args[0];
		palloc_free_page(args);

		lock_acquire(&file_lock);
		f->eax = filesize(a60);
		lock_release(&file_lock);
		break;

	case SYS_READ:
		check_address_valid((char*)stack_pt);
		check_address_valid((char*)stack_pt + 4);
		check_address_valid((char*)stack_pt + 8);
		patch_argument(stack_pt, args, 3);

		check_address_valid((void*)args[1]);

		int a70 = args[0];
		void* a71 = args[1];
		unsigned a72 = args[2];
		palloc_free_page(args);

		lock_acquire(&file_lock);
		f->eax = read(a70, a71, a72);
		lock_release(&file_lock);
		break;

	case SYS_WRITE:
		check_address_valid((char*)stack_pt);
		check_address_valid((char*)stack_pt + 4);
		check_address_valid((char*)stack_pt + 8);
		patch_argument(stack_pt, args, 3);

		check_address_valid((const void*)args[1]);

		int a80 = args[0];
		void* a81 = args[1];
		unsigned a82 = args[2];
		palloc_free_page(args);

		lock_acquire(&file_lock);
		f->eax = write(a80, a81, a82);
		lock_release(&file_lock);
		break;

	case SYS_SEEK:
		check_address_valid((char*)stack_pt);
		check_address_valid((char*)stack_pt + 4);
		patch_argument(stack_pt, args, 2);

		int a90 = args[0];
		unsigned a91 = args[1];
		palloc_free_page(args);

		lock_acquire(&file_lock);
		seek(a90, a91);
		lock_release(&file_lock);
		break;

	case SYS_TELL:
		check_address_valid((char*)stack_pt);
		patch_argument(stack_pt, args, 1);

		int aa0 = args[0];
		palloc_free_page(args);

		lock_acquire(&file_lock);
		f->eax = tell(aa0);
		lock_release(&file_lock);
		break;

	case SYS_CLOSE:
		check_address_valid((char*)stack_pt);
		patch_argument(stack_pt, args, 1);

		int ab0 = args[0];
		palloc_free_page(args);

		lock_acquire(&file_lock);
		close(ab0);
		lock_release(&file_lock);
		break;

	default:
		palloc_free_page(args);
		exit(-1);
		break;
	}
}

/* Project2 Edited */
/* Check if the memory address is valid. */
void check_address_valid(void* addr) {
	//user memory is 0x8048000~0xc0000000 in pintos
	if (!is_user_vaddr(addr)) {
		exit(-1);
	}
}

/* Project2 Edited */
/* patch argument from user stack */
void patch_argument(void* esp, int* arg ,int count) {
	void* pt = esp;
	for (int i = 0; i < count; i++) {
		check_address_valid(pt);
		arg[i] = *(int*)pt;
		pt = (int*)pt + 1;
	}
}

/* Project2 Edited */
void halt() {
	shutdown_power_off();
}

/* Project2 Edited */
void exit(int status) {
	struct thread* cur = thread_current();
	cur->exit_status = status;
	printf("%s: exit(%d)\n",thread_current()->name, status);
	thread_exit();
}

/* Project2 Edited */
tid_t exec(const char * cmdline) {
	check_address_valid(cmdline);
	tid_t pid = process_execute(cmdline);
	struct thread* child = get_child(pid);
	return pid;
}

/* Project2 Edited */
int wait(tid_t pid) {
	return process_wait(pid);
}

/* Project2 Edited */
int p_open_file(struct file* file) {
	struct thread* cur = thread_current();
	struct fd_elem* f;

	if (file == NULL) {
		exit(-1);
	}

	f = palloc_get_page(0);

	f->fd = cur->fd_size;
	f->file = file;
	list_push_back(&(cur->fd_list), &(f->elem));

	cur->fd_size++;
	return f->fd;
}

/* Project2 Edited */
struct file* p_find_file(int fd) {
	struct thread* cur = thread_current();
	struct list_elem *e;
	for (e = list_begin (&(cur->fd_list)); e != list_end (&(cur->fd_list)); e = list_next (e)) {
		struct fd_elem* f = list_entry(e, struct fd_elem, elem);
		if (f->fd == fd) return f->file;
	}
	return NULL;
} 

/* Project2 Edited */
void p_close_file(int fd) {
	struct thread* cur = thread_current();
	struct list_elem *e;
	struct fd_elem* f;

	if (fd == -1) {
		while (!list_empty(&(cur->fd_list))) {
			cur->fd_size--;
			e = list_front(&(cur->fd_list));
			f = list_entry(e, struct fd_elem, elem);
			list_remove(e);
			file_close(f->file);
			palloc_free_page(f);
		}
	}
	else {
		for (e = list_begin (&(cur->fd_list)); e != list_end (&(cur->fd_list)); e = list_next (e)) {
			cur->fd_size--;
			f = list_entry(e, struct fd_elem, elem);
			if (f->fd == fd) {
				list_remove(e);
				file_close(f->file);
				palloc_free_page(f);
				break;
			}
		}
	}
}

/* Project2 Edited */
bool create(const char* file, unsigned file_size) {
	bool success;
	if (file == NULL) {
		exit(-1);
	}
	success = filesys_create(file,file_size);
	return success;
}

/* Project2 Edited */
bool remove(const char *file) {
	return filesys_remove(file);
}

/* Project2 Edited */
int open(const char *file) {
	if (file == NULL) exit(-1);
	struct file* f = filesys_open(file);
	struct thread* t = thread_current();
	if (strcmp(t->name, file) == 0) file_deny_write(f);
	if (f == NULL) return -1;
	return p_open_file(f);

}

/* Project2 Edited */
int filesize(int fd) {
	struct file* f = p_find_file(fd);
	if (f == NULL) exit(-1);
	return file_length(f);
}

/* Project2 Edited */
int read(int fd, void* buffer, unsigned size) {
	int i;
	if (fd == F_STDIN) {
		for (i = 0; i < size; i++) ((char*)buffer)[i] = input_getc();
		return size;
	}
	else {
		struct file* f = p_find_file(fd);
		if (f == NULL) return -1;
		return file_read(f, buffer, size);
	}
}

/* Project2 Edited */
int write(int fd, const void* buffer, unsigned size) {
	if (fd == F_STDOUT) {
		putbuf(buffer, size);
		return size;
	}
	else {
		struct file* f = p_find_file(fd);
		if (f == NULL) return -1;
		return file_write(f, buffer, size);
	}
}

/* Project2 Edited */
void seek (int fd, unsigned position) {
	struct file* f = p_find_file(fd);
	if (f == NULL) exit(-1);
	file_seek(f, position);
}

/* Project2 Edited */
unsigned tell (int fd) {
	struct file* f = p_find_file(fd);
	if (f == NULL) exit(-1);
	return file_tell(f);
}

/* Project2 Edited */
void close (int fd) {
	p_close_file(fd);
}