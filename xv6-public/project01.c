#include "types.h"
#include "stat.h"
#include "user.h"

int
main(int argc, char *argv[])
{
    printf(1, "My pid is %d\n",getpid());
    printf(1, "My ppid is %d\n",getppid());
    exit();
}