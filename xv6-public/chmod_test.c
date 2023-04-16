#include "types.h"
#include "stat.h"
#include "user.h"
#include "fs.h"

/*
<HOW TO USE>
xv6 부팅 후, 아래 명령어를 입력해 실행하시면 됩니다.
chmod OO XXX
- OO에는 각각 owner와 others를 위한 mode를 입력하면 됩니다.
- read(4), write(2), execute(1) 중 원하는 권한들의 숫자를 더해서 입력하면 됩니다.
- ex. 3(w+e) 5(r+e) 7(r+w+e)
- XXX에는 권한을 변경하고 싶은 파일을 입력합니다.
*/
int main(int argc, char *argv[])
{
  int mode = 0;
  if (argc < 3)
  {
    printf(1, "Usage: chmod mode target\n");
    printf(1, "mode: AB where A,B=[0,7]. A is for the owner and B is for others.\n");
    printf(1, "Each number is a sum of three options:\n");
    printf(1, "  4: read\n");
    printf(1, "  2: write\n");
    printf(1, "  1: execute\n\n");
    exit();
  }
  if (strlen(argv[1]) != 2 || argv[1][0] < '0' || argv[1][0] > '7' || argv[1][1] < '0' || argv[1][1] > '7')
  {
    printf(1, "Invalid mode: %s\n", argv[1]);
    exit();
  }
  argv[1][0] -= '0';
  argv[1][1] -= '0';
  if (argv[1][0] & 4)
    mode |= MODE_RUSR;
  if (argv[1][0] & 2)
    mode |= MODE_WUSR;
  if (argv[1][0] & 1)
    mode |= MODE_XUSR;
  if (argv[1][1] & 4)
    mode |= MODE_ROTH;
  if (argv[1][1] & 2)
    mode |= MODE_WOTH;
  if (argv[1][1] & 1)
    mode |= MODE_XOTH;

  if (chmod(argv[2], mode) == 0)
    printf(1, "chmod successful\n");
  else
    printf(1, "chmod failed!\n");

  exit();
}

