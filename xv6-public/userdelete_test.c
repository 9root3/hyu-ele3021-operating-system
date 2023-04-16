#include "types.h"
#include "stat.h"
#include "user.h"

int main(int argc, char *argv[])
{
  char username[50];
  printf(1, "[Delete user]\n");

  printf(1, "Username: ");
  gets(username, sizeof username);
  username[strlen(username) - 1] = 0;
  
  if (deleteUser(username) == 0)
    printf(1, "Delete user successful!\n");
  else
    printf(1, "Delete user failed!\n");
  exit();
}

