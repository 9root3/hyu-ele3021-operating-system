#include "types.h"
#include "stat.h"
#include "user.h"

#define NUM_LOOP 20
#define NUM_YIELD 50000
#define NUM_SLEEP 10

#define NUM_THREAD 4

int parent;

int fork_children()
{
  int i, p;
  for (i = 0; i < NUM_THREAD; i++)
    if ((p = fork()) == 0)
    {
      sleep(10);
      return getpid();
    }
  return parent;
}

void exit_children()
{
  if (getpid() != parent)
    exit();
  while (wait() != -1);
}

int main(int argc, char *argv[])
{
  int i, pid;

  parent = getpid();

  printf(1, "Multilevel test start\n");

  printf(1, "[Test 1] without yield / sleep\n");
  pid = fork_children();

  if (pid != parent)
  {
    for (i = 0; i < NUM_LOOP; i++)
      printf(1, "Process %d\n", pid);
  }
  exit_children();
  printf(1, "[Test 1] finished\n");

  printf(1, "[Test 2] with yield\n");
  pid = fork_children();

  if (pid != parent)
  {
    for (i = 0; i < NUM_YIELD; i++)
      yield();
    printf(1, "Process %d finished\n", pid);
  }
  
  exit_children();
  printf(1, "[Test 2] finished\n");

  printf(1, "[Test 3] with sleep\n");
  pid = fork_children();

  if (pid != parent)
  {
    for (i = 0; i < NUM_SLEEP; i++)
    {
      printf(1, "Process %d\n", pid);
      sleep(10);
    }
  }
  exit_children();
  printf(1, "[Test 3] finished\n");
  exit();
}

