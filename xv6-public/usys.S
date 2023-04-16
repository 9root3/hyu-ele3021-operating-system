#include "syscall.h"
#include "traps.h"

#define SYSCALL(name) \
  .globl name; \
  name: \
    movl $SYS_ ## name, %eax; \
    int $T_SYSCALL; \
    ret

SYSCALL(fork)
SYSCALL(exit)
SYSCALL(wait)
SYSCALL(pipe)
SYSCALL(read)
SYSCALL(write)
SYSCALL(close)
SYSCALL(kill)
SYSCALL(exec)
SYSCALL(open)
SYSCALL(mknod)
SYSCALL(unlink)
SYSCALL(fstat)
SYSCALL(link)
SYSCALL(mkdir)
SYSCALL(chdir)
SYSCALL(dup)
SYSCALL(getpid)
SYSCALL(getppid)
SYSCALL(sbrk)
SYSCALL(sleep)
SYSCALL(uptime)
SYSCALL(myfunction)
SYSCALL(yield)
SYSCALL(getlev)
SYSCALL(setpriority)  
SYSCALL(thread_create)
SYSCALL(thread_exit)
SYSCALL(thread_join)
SYSCALL(initUser)
SYSCALL(checkUser)
SYSCALL(addUser)
SYSCALL(deleteUser)
SYSCALL(fork2)
SYSCALL(chmod)
SYSCALL(whoami)
