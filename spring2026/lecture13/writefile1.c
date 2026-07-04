#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>

// this program will take two arguments
// the first will be a count
// the second will be a file name
// and then we write the 0-count to the file
// with a newline after every number
int main(int argc, char* argv[]){

  if(argc < 3){
    printf("Please take this seriously\n");
    return 1;
  }

  int count = atoi(argv[1]);
  int fd = open(argv[2], O_WRONLY | O_CREAT | O_TRUNC, 0666);
  // middle argument are flags
  // O_WRONLY
  // O_RDONLY
  // O_RDWR
  // if you want the file to be created if it doesn't
  // exist you have to include O_CREAT
  // if you want it to be truncated to 0 before writing
  // again you need to add O_TRUNC
  // then the last argument is the permissions, which if you're comfortable with Linux permission in octal
  // i.e. O000 - O777
  // you can just give the argument in octal
  // O666 means every gets read and write permissions

  if(fd < 0){
    printf("We couldn't open the file\n");
    return 1;
  }
  
  for(int i =0; i<=count; i++){
    // we want to first build our string
    // to be printed to the file
    // then we need to use write to do it
    char buf[100];
    // to build the string let's use 
    snprintf(buf,sizeof(buf),"%d\n",i);

    // now we write
    write(fd,buf,strlen(buf)); // I accidentally put sizeof(buf) here not strlen(buf)
    // is this good enough?
    // writes don't always write out everything they can in one go
    // so similarly to reads you may have to put in a loop so that you don't leave some of the buffer unwritten
    // 
  }
  close(fd);
  
  return 0;
}
