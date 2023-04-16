#include "types.h"
#include "stat.h"
#include "user.h"

int main(int argc, char *argv[])
{
  char username[50], password[50];
  printf(1, "[Add user]\n");

  printf(1, "Username: ");
  gets(username, sizeof username);
  username[strlen(username) - 1] = 0;
  
  printf(1, "Password: ");
  gets(password, sizeof password);
  password[strlen(password) - 1] = 0;

  if (addUser(username, password) == 0)
    printf(1, "Add user successful!\n");
  else
    printf(1, "Add user failed!\n");
  exit();
}

