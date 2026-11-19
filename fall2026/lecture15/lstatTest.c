#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>

int main(int argc, char* argv[]){
  int fd = open(argv[1], O_RDONLY);

  struct stat file_stat;
  
  if(fd == -1){
    printf("Whoops that file can't be opened!\n");
    return 1;
  }
  if(lstat(argv[1], &file_stat) == -1){
    printf("Oh something's wrong with that file.\n");
    return 1;
  }

  printf("File type: ");
  switch (file_stat.st_mode & S_IFMT) {
   case S_IFREG:  printf("regular file\n"); break;
   case S_IFDIR:  printf("directory\n"); break;
   case S_IFLNK:  printf("symbolic link\n"); break;
   case S_IFSOCK: printf("socket\n"); break;
   case S_IFBLK:  printf("block device\n"); break;
   case S_IFCHR:  printf("character device\n"); break;
   case S_IFIFO:  printf("FIFO/pipe\n"); break;
   default:       printf("unknown\n"); break;
  }


  printf("%c", S_ISDIR(file_stat.st_mode) ? 'd' : '-');
  printf("%c", file_stat.st_mode & S_IRUSR ? 'r' : '-');
  printf("%c", file_stat.st_mode & S_IWUSR ? 'w' : '-');
  printf("%c", file_stat.st_mode & S_IXUSR ? 'x' : '-');
  printf("%c", file_stat.st_mode & S_IRGRP ? 'r' : '-');
  printf("%c", file_stat.st_mode & S_IWGRP ? 'w' : '-');
  printf("%c", file_stat.st_mode & S_IXGRP ? 'x' : '-');
  printf("%c", file_stat.st_mode & S_IROTH ? 'r' : '-');
  printf("%c", file_stat.st_mode & S_IWOTH ? 'w' : '-');
  printf("%c", file_stat.st_mode & S_IXOTH ? 'x' : '-');
  
  printf(" (%o)\n", file_stat.st_mode & 0777);
  
  printf("Size of file in bytes: %ld\n", (long) file_stat.st_size);

  printf("\nAccess time: %s", ctime(&file_stat.st_atime));
  printf("Modify time: %s", ctime(&file_stat.st_mtime));
  printf("Change time: %s", ctime(&file_stat.st_ctime));
  
  return 0;
}
