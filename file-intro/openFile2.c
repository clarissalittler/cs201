#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char* argv[]) {
    int fd = open(argv[1], O_RDONLY);

    if (fd == -1){
      printf("Whoops that file can't be opened!\n");
      return 1;
    }
    char buffer[1024];
    ssize_t bytes_read;

    while ((bytes_read = read(fd, buffer, sizeof(buffer))) > 0) {
        write(STDOUT_FILENO, buffer, bytes_read);  
    }

    if(bytes_read == -1){
      printf("Oh dag, can't actually read from that file\n");
      close(fd);
      return 1;
    }
    
    close(fd);
    return 0;
}
