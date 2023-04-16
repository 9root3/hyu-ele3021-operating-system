#include "types.h"
#include "stat.h"
#include "user.h"

#define NUM_THREAD 5

void *thread1(void *arg)
{
  sleep(200);
  printf(1, "This code shouldn't be executed!!\n");
  exit();
  return 0;
}

void *thread2(void *arg)
{
  int val = (int)arg;
  sleep(100);
  if (val != 0) {
    printf(1, "Killing process %d\n", val);
    kill(val);
  }
  printf(1, "This code should be executed 5 times.\n");
  thread_exit(0);
  return 0;
}

thread_t t1[NUM_THREAD], t2[NUM_THREAD];

int main(int argc, char *argv[])
{
  int i, retval;
  int pid;
  
  printf(1, "Thread kill test start\n");
  pid = fork();
  if (pid < 0) {
    printf(1, "Fork failed!!\n");
    exit();
  }
  else if (pid == 0) {
    for (i = 0; i < NUM_THREAD; i++)
      thread_create(&t1[i], thread1, (void *)i);
    sleep(300);
    printf(1, "This code shouldn't be executed!!\n");
    exit();
  }
  else {
    for (i = 0; i < NUM_THREAD; i++) {
      if (i == 0)
        thread_create(&t2[i], thread2, (void *)pid);
      else
        thread_create(&t2[i], thread2, (void *)0);
    }
    for (i = 0; i < NUM_THREAD; i++)
      thread_join(t2[i], (void **)&retval);
    while (wait() != -1)
      ;
    printf(1, "Kill test finished\n");
    exit();
  }
}
