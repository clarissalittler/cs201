// Everything is a file, part 4: the descriptor table
//
// A file descriptor is just an index into this process's table of
// open things. Regular files, pipes, terminals, and devices all live in
// the same table and get numbers the same way: the lowest free slot.
//
// Linux lets us *look* at our own table: /proc/self/fd is a directory
// with one entry per open descriptor.
//
// try:
//   ./fdTable
//   ./fdTable < sample.txt > out.txt   (then cat out.txt -- what are 0 and 1 now?)

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>

int main(void){
  int fileFd = open("sample.txt", O_RDONLY);

  int pipeFds[2];
  pipe(pipeFds);                              // two descriptors: [0] read end, [1] write end

  int nullFd = open("/dev/null", O_WRONLY);   // a device

  printf("file   -> fd %d\n", fileFd);
  printf("pipe   -> fds %d and %d\n", pipeFds[0], pipeFds[1]);
  printf("device -> fd %d\n", nullFd);
  printf("\nwhat /proc/self/fd says is open:\n");

  // opendir itself needs a descriptor, so it will show up in the list too!
  DIR* dir = opendir("/proc/self/fd");
  struct dirent* entry;
  while((entry = readdir(dir)) != NULL){
    if(entry->d_name[0] == '.') continue;     // skip . and ..

    // each entry is a symbolic link; readlink tells us what it points to
    char path[300];
    char target[256];
    snprintf(path, sizeof(path), "/proc/self/fd/%s", entry->d_name);
    ssize_t n = readlink(path, target, sizeof(target) - 1);
    if(n < 0) continue;
    target[n] = '\0';                         // readlink doesn't add one

    printf("  fd %-3s -> %s\n", entry->d_name, target);
  }
  closedir(dir);

  close(fileFd);
  close(pipeFds[0]);
  close(pipeFds[1]);
  close(nullFd);

  // question: if we open another file right now, what number will it get?
  return 0;
}
