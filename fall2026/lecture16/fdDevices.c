// Everything is a file, part 3: devices
//
// Files under /dev aren't data sitting on a disk. Opening one connects
// you to a driver in the kernel, and read/write ask that driver to do
// something.
//
//   /dev/urandom   read gives you random bytes, forever
//   /dev/zero      read gives you zero bytes, forever
//   /dev/null      read gives you EOF; write throws everything away
//
// try:
//   ./fdDevices
//   ls -l /dev/null /dev/zero /dev/urandom sample.txt
//     (look at the first letter: 'c' means character device, '-' means regular file)

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

void showBytes(const char* label, unsigned char* buf, ssize_t n){
  printf("%-14s", label);
  for(ssize_t i = 0; i < n; i++){
    printf("%02x ", buf[i]);
  }
  printf("(%zd bytes)\n", n);
}

int main(void){
  unsigned char buf[8];
  ssize_t n;

  int r = open("/dev/urandom", O_RDONLY);
  n = read(r, buf, sizeof(buf));
  showBytes("/dev/urandom", buf, n);   // run it twice: different every time
  close(r);

  int z = open("/dev/zero", O_RDONLY);
  n = read(z, buf, sizeof(buf));
  showBytes("/dev/zero", buf, n);
  close(z);

  int nul = open("/dev/null", O_RDWR);
  n = read(nul, buf, sizeof(buf));
  printf("%-14sread returned %zd (EOF immediately)\n", "/dev/null", n);
  n = write(nul, "goodbye, bytes", 14);
  printf("%-14swrite returned %zd (it \"wrote\" everything...somewhere)\n",
         "/dev/null", n);
  close(nul);

  return 0;
}
