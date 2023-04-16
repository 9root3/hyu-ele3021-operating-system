#include "types.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "mmu.h"
#include "x86.h"
#include "spinlock.h"
#include "sleeplock.h"
#include "proc.h"

#include "fs.h"
#include "file.h"
#include "stat.h"

#define NUM_USER 10
#define USER_FIELD 32

struct {
  struct spinlock lock;
  struct inode * ptr;
  char u[NUM_USER][USER_FIELD];
  int status[NUM_USER];
} utable;

extern struct proc* myproc(void);

int load_shadow(char *buf);

char*
strcpy(char *s, const char *t)
{
  char *os;
  os = s;
  while((*s++ = *t++) != 0);
  return os;
}

int
init_utable(void)
{
  int i, j;

  initlock(&utable.lock, "utable");

  acquire(&utable.lock);
  for(i = 0; i < NUM_USER; i++){
    for(j = 0; j < USER_FIELD; j++)
      utable.u[i][j] = '\0';
    utable.status[i] = 0;
  }
  release(&utable.lock);
  cprintf("init utable success\n");
  return 0;
}

int
initUser(void)
{
  cprintf("start init user table\n");
  init_utable();
  return load_shadow(&(utable.u[0][0]));
}

int
sys_initUser(void)
{
  return initUser();
}

int 
load_shadow(char *buf)
{
  char *path = "/shadow";
  struct inode *ip, *dp;
  char name[DIRSIZ];

  begin_op();

  if((dp = nameiparent(path, name)) == 0)
    return -1;
  
  ilock(dp);

  if((ip = dirlookup(dp, name, 0)) == 0){
    acquire(&utable.lock);

    strcpy(utable.u[0], "root");
    strcpy(utable.u[0] + USER_FIELD/2, "0000");
    utable.status[0] = 1;

    release(&utable.lock);

    ip = ialloc(dp->dev, T_FILE);

    ilock(ip);
    ip->major = 0;
    ip->minor = 0;
    ip->nlink = 1;
    strcpy(ip->userid, "root");
    iupdate(ip);

    dirlink(dp, name, ip->inum);

    iunlockput(dp);

    if(writei(ip, &(utable.u[0][0]), 0, sizeof(utable.u)) < 0){
      iunlock(ip);
      end_op();
      return -1;
    }

    iunlock(ip);

  } else {
    iunlockput(dp);
    if((ip = namei(path)) == 0){
      end_op();
      return -1;
    }
  }

  acquire(&utable.lock);
  utable.ptr = ip;
  release(&utable.lock);

  ilock(ip);

  if(readi(ip, buf, 0, sizeof(utable.u)) < 0){
    end_op();
    return -1;
  }

  iunlock(ip);
  end_op();

  return 0;
}

extern int strcmp(const char *p, const char *q);

int
checkUser(char *username, char *password)
{
  int i;

  for(i = 0; i < NUM_USER; i++){

    if(!strcmp(&(utable.u[i][0]), username) && !strcmp(&(utable.u[i][0]) + USER_FIELD/2, password)) {
      return 1;
    }
  }
  return 0;
}

int
addUser(char *username, char *password)
{
  struct proc *curproc = myproc();
  int i;

  char utemp[NUM_USER][USER_FIELD] = {0, };

  if(!strcmp(curproc->userid, "root"))
    ;
  else
    return -1;

  acquire(&utable.lock);

  for(i = 0; i < NUM_USER; i++){
    if(!strcmp(&(utable.u[i][0]), username)) {
      return -1;
    }
  }

  for(i = 0; i < NUM_USER; i++){
    if(utable.status[i] == 0)
      break;
  }

  if(i == NUM_USER || strlen(username) > USER_FIELD/2 || strlen(username) < 0 || strlen(password) > USER_FIELD/2 || strlen(password) < 0){
    return -1;
  }

  strcpy(&(utable.u[i][0]), username);
  strcpy(&(utable.u[i][0]) + USER_FIELD/2, password);
  utable.status[i] = 1;

  memmove(&(utemp[0][0]), &(utable.u[0][0]), NUM_USER*USER_FIELD);
  release(&utable.lock);

  begin_op();

  ilock(utable.ptr);

  if(writei(utable.ptr, &(utemp[0][0]), 0, sizeof(utemp)) < 0){
    iunlock(utable.ptr);
    end_op();
    return -1;
  }

  iunlock(utable.ptr);

  // directory

  struct inode *fp, *dp;

  if((dp = namei("/")) == 0)
    return -1;

  if((fp = dirlookup(dp, username, 0)) == 0){
    fp = ialloc(dp->dev, T_DIR);

    ilock(dp);
    ilock(fp);
    fp->major = 0;
    fp->minor = 0;
    fp->nlink = 1;
    strcpy(fp->userid, username);
    iupdate(fp);

    dp->nlink++;  // for ".."
    iupdate(dp);

    // No ip->nlink++ for ".": avoid cyclic ref count.
    if(dirlink(fp, ".", fp->inum) < 0 || dirlink(fp, "..", dp->inum) < 0)
      panic("error in load_shadow(): create dots");

    dirlink(dp, username, fp->inum);

    iupdate(fp);
    iupdate(dp);

    iunlockput(fp);
    iunlockput(dp);
  }
  
  end_op();

  return 0;
}

int
deleteUser(char *username)
{
  struct proc *curproc = myproc();
  int i, j;

  char utemp[NUM_USER][USER_FIELD] = {0, };

  if(!strcmp(curproc->userid, "root"))
    ;
  else
    return -1;

  if(!strcmp(username, "root")){
    return -1;
  }

  acquire(&utable.lock);

  for(i = 0; i < NUM_USER; i++){
    if(!strcmp(&(utable.u[i][0]), username)) {
      for(j = 0; j < USER_FIELD; j++) {
        utable.u[i][j] = '\0';
      }

      utable.status[i] = 0;

      memmove(&(utemp[0][0]), &(utable.u[0][0]), NUM_USER*USER_FIELD);
      release(&utable.lock);

      begin_op();

      ilock(utable.ptr);

      if(writei(utable.ptr, &(utemp[0][0]), 0, sizeof(utemp)) < 0){
        end_op();
        return -1;
      }

      iunlock(utable.ptr);
      end_op();

      return 0;
    }
  }

  release(&utable.lock);
  return -1;
}

int
chmod(char *pathname, int mode)
{
  struct inode *ip;
  begin_op();
  if((ip = namei(pathname)) == 0) {
    end_op();
    cprintf("chmod: invalid path\n");
    return -1;
  }
  ilock(ip);
  if((strcmp(myproc()->userid, "root") != 0) && (strcmp(myproc()->userid, ip->userid) != 0)){
    cprintf("chmod: privilege error (only owner of the file or root!)\n");
    iunlock(ip);
    end_op();
    return -1;
  }
  if(ip->type == T_DEV) {
    cprintf("chmod: cannot change T_DEV (e.g. console)\n");
    iunlock(ip);
    end_op();
    return -1;
  }
  ip->mode = mode;
  iupdate(ip);
  iunlock(ip);
  end_op();
  return 0;
}

int
whoami(char *username)
{
  strcpy(username, myproc()->userid);
  return 0;
}

int
sys_checkUser(void)
{
  char *username, *password;
  if(argstr(0, &username) < 0 || argstr(1, &password) < 0)
    return -1;
  return checkUser(username, password);
}

int
sys_addUser(void)
{
  char *username, *password;
  if(argstr(0, &username) < 0 || argstr(1, &password) < 0)
    return -1;
  return addUser(username, password);
}

int
sys_deleteUser(void)
{
  char *username;
  if(argstr(0, &username) < 0)
    return -1;
  return deleteUser(username);
}

int
sys_chmod(void)
{
  char *pathname;
  int mode;
  if(argstr(0, &pathname) < 0 || argint(1, &mode) < 0)
    return -1;
  return chmod(pathname, mode);
}

int sys_whoami(void)
{
  char *username;
  if(argstr(0, &username) < 0)
    return -1;
  return whoami(username);
}