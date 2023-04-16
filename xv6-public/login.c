#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"

#define MAXLEN 15

char id[MAXLEN + 5];
char pw[MAXLEN + 5];

int
getinput(char *buf, int nbuf)
{
  memset(buf, 0, nbuf);
  gets(buf, nbuf);
  if(buf[0] == 0) // EOF
    return -1;
  return 0;
}

int
prompt(void)
{
  // get user input
  printf(1, "ID: ");
  getinput(id, 20);

  printf(1, "PW: ");
  getinput(pw, 20);

  if(id[0] == '\0' || id[0] == '\n' || pw[0] == '\n') {
    return -1;
  }

  // parsing
  if(id[strlen(id) - 1] == '\n')
    id[strlen(id) - 1] = '\0';
  else
    id[strlen(id)] = '\0';

  if(pw[strlen(pw) - 1] == '\n')
    pw[strlen(pw) - 1] = '\0';
  else
    pw[strlen(pw)] = '\0';

  return checkUser(id, pw);
}

int
main(int argc, char *argv[])
{
  int pid, wpid;
  int flag = 0;

  for(;;){

    while((flag = prompt()) < 1) {
      switch(flag) {
      case -1:
        printf(1, "login: you forgot to type ID or PW\n");
        break;
      case 0:
        printf(1, "login: ID does not exist or wrong password\n");
        break;
      default:
        printf(1, "login: unknown error occured\n");
        break;
      }
    }

    char *args[] = { "sh", id, 0 };

    pid = fork2(id);
    if(pid < 0){
      printf(1, "login: fork failed\n");
      exit();
    }
    if(pid == 0){
      exec("sh", args);
      printf(1, "login: exec sh failed\n");
      exit();
    }
    while((wpid=wait()) >= 0 && wpid != pid)
      printf(1, "zombie!\n");
  }
}
