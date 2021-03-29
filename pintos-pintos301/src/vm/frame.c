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
