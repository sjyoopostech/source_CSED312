
#include "userprog/syscall.h"
#include "threads/vaddr.h"

static void syscall_handler (struct intr_frame *);

struct lock file_lock;

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
	check_address_valid(stack_pt);

	patch_argument(stack_pt, args, 1);

	sysnum = args[0];

	stack_pt = (int*)stack_pt + 1;

	switch (sysnum){
	case SYS_HALT:
		halt();
		break;
	case SYS_EXIT:
		check_address_valid(stack_pt);
		patch_argument(stack_pt, args, 1);
		exit(args[0]);
		break;
	case SYS_EXEC:
		patch_argument(stack_pt, args, 1);
		lock_acquire(&file_lock);
		check_address_valid(stack_pt);
		f->eax = exec((const char*)*(uint32_t*)(stack_pt));
		lock_release(&file_lock);
		break;
	case SYS_WAIT:
		patch_argument(stack_pt, args, 1);
		check_address_valid(stack_pt);
		f->eax = wait((tid_t)*(uint32_t*)(stack_pt));
		break;
	case SYS_CREATE:
		//TODO
		check_address_valid((char*)stack_pt);
		check_address_valid((char*)stack_pt + 4);
		patch_argument(stack_pt, args, 2); // name and size of file
		char* file = args[0];
		unsigned file_size = args[1];
		check_address_valid(file);
		f->eax=create(file, file_size);
		break;
	case SYS_REMOVE:
		patch_argument(stack_pt, args, 1); // name and size of file
		char* filename = args[0];
		check_address_valid(filename);
		remove(filename);
		break;
	case SYS_OPEN:
		break;
	case SYS_FILESIZE:
		break;
	case SYS_READ:
		check_address_valid(stack_pt + 16);
		check_address_valid(stack_pt + 20);
		check_address_valid(stack_pt + 24);
		patch_argument(stack_pt, args, 3);
		read((int)*(uint32_t*)(stack_pt + 20), (void*)*(uint32_t*)(stack_pt + 24), (unsigned)*((uint32_t*)(stack_pt + 28)));
		break;
	case SYS_WRITE:
		check_address_valid(stack_pt + 16);
		check_address_valid(stack_pt + 20);
		check_address_valid(stack_pt + 24);
		patch_argument(stack_pt, args, 3);
		write((int)args[0], (void*)args[1], (unsigned)args[2]);
		break;
	case SYS_SEEK:
		break;
	case SYS_TELL:
		break;
	case SYS_CLOSE:
		break;
	default:
		exit(-1);
		break;
	}
	palloc_free_page(args);
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

tid_t exec(const char * cmdline) {
	check_address_valid(cmdline);
	tid_t pid = process_execute(cmdline);
	struct thread* child = get_child(pid);
	return pid;
}

int wait(tid_t pid) {
	return process_wait(pid);
}

/* Project2 Edited */
bool create(const char* file, unsigned file_size) {
	bool success;
	if (file == NULL) {
		exit(-1);
	}
	lock_acquire(&file_lock);
	success = filesys_create(file,file_size);
	lock_release(&file_lock);
	return success;
}

/* Project2 Edited */
bool remove(const char *file) {
	return filesys_remove(file);
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


int read(int fd, void* buffer, unsigned size) {
	int i;
	if (fd == 0) {
		for (i = 0; i < size; i++) {
			if (((char*)buffer)[i] == '\0') {
				break;
			}
		}
	}
	return i;
}

int write(int fd, const void* buffer, unsigned size) {


	if (fd == 1) {
		putbuf(buffer, size);
		return size;
	}
	return -1;
}
