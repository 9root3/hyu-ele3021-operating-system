#include "types.h"
#include "stat.h"
#include "user.h"

#define NUM_THREAD 5

int status;
thread_t thread[NUM_THREAD];
int expected[NUM_THREAD];

void failed()
{
  printf(1, "Test failed!\n");
  exit();
}

void *thread_basic(void *arg)
{
  int val = (int)arg;
  printf(1, "Thread %d start\n", val);
  if (val == 1) {
    sleep(200);
    status = 1;
  }
  printf(1, "Thread %d end\n", val);
  thread_exit(arg);
  return 0;
}

void *thread_fork(void *arg)
{
  int val = (int)arg;
  int pid;

  printf(1, "Thread %d start\n", val);
  pid = fork();
  if (pid < 0) {
    printf(1, "Fork error on thread %d\n", val);
    failed();
  }

  if (pid == 0) {
    printf(1, "Child of thread %d start\n", val);
    sleep(100);
    status = 3;
    printf(1, "Child of thread %d end\n", val);
    exit();
  }
  else {
    status = 2;
    if (wait() == -1) {
      printf(1, "Thread %d lost their child\n", val);
      failed();
    }
  }
  printf(1, "Thread %d end\n", val);
  thread_exit(arg);
  return 0;
}

int *ptr;

void *thread_sbrk(void *arg)
{
  int val = (int)arg;
  printf(1, "Thread %d start\n", val);

  int i, j;

  if (val == 0) {
    ptr = (int *)malloc(65536);
    sleep(100);
    free(ptr);
    ptr = 0;
  }
  else {
    while (ptr == 0)
      sleep(1);
    for (i = 0; i < 16384; i++)
      ptr[i] = val;
  }

  while (ptr != 0)
    sleep(1);

  for (i = 0; i < 2000; i++) {
    int *p = (int *)malloc(65536);
    for (j = 0; j < 16384; j++)
      p[j] = val;
    for (j = 0; j < 16384; j++) {
      if (p[j] != val) {
        printf(1, "Thread %d found %d\n", val, p[j]);
        failed();
      }
    }
    free(p);
  }

  thread_exit(arg);
  return 0;
}
void create_all(int n, void *(*entry)(void *))
{
  int i;
  for (i = 0; i < n; i++) {
    if (thread_create(&thread[i], entry, (void *)i) != 0) {
      printf(1, "Error creating thread %d\n", i);
      failed();
    }
  }
}

void join_all(int n)
{
  int i, retval;
  for (i = 0; i < n; i++) {
    if (thread_join(thread[i], (void **)&retval) != 0) {
      printf(1, "Error joining thread %d\n", i);
      failed();
    }
    if (retval != expected[i]) {
      printf(1, "Thread %d returned %d, but expected %d\n", i, retval, expected[i]);
      failed();
    }
  }
}

int main(int argc, char *argv[])
{
  int i;
  for (i = 0; i < NUM_THREAD; i++)
    expected[i] = i;

  printf(1, "Test 1: Basic test\n");
  create_all(2, thread_basic);
  sleep(100);
  printf(1, "Parent waiting for children...\n");
  join_all(2);
  if (status != 1) {
    printf(1, "Join returned before thread exit, or the address space is not properly shared\n");
    failed();
  }
  printf(1, "Test 1 passed\n\n");

  printf(1, "Test 2: Fork test\n");
  create_all(NUM_THREAD, thread_fork);
  join_all(NUM_THREAD);
  if (status != 2) {
    if (status == 3) {
      printf(1, "Child process referenced parent's memory\n");
    }
    else {
      printf(1, "Status expected 2, found %d\n", status);
    }
    failed();
  }
  printf(1, "Test 2 passed\n\n");

  printf(1, "Test 3: Sbrk test\n");
  create_all(NUM_THREAD, thread_sbrk);
  join_all(NUM_THREAD);
  printf(1, "Test 3 passed\n\n");

  printf(1, "All tests passed!\n");
  exit();
}
