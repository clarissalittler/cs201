// socketpair(): two Unix sockets already connected to each other, with no
// file name and no bind/listen/accept.
//
// It's like pipe() from the file-descriptor lecture, with two differences:
//   - a pipe goes ONE way (fd[0] reads, fd[1] writes). Each end of a
//     socket pair can both send and receive.
//   - so one pair is enough for a conversation in both directions. With
//     pipes, the parent and child would need two pipes.
//
// Here the parent sends lines to the child, the child shouts them back in
// upper case, and the parent prints the replies.
//
// Build:   make socketPair
// Run:     ./socketPair

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/wait.h>

int main(void){
  int sv[2];
  if(socketpair(AF_UNIX, SOCK_STREAM, 0, sv) < 0){
    perror("socketpair");
    return 1;
  }

  pid_t pid = fork();
  if(pid == 0){
    // child: keep sv[1], close the parent's end
    close(sv[0]);
    char buf[256];
    ssize_t n;
    while((n = recv(sv[1], buf, sizeof(buf), 0)) > 0){
      for(ssize_t i = 0; i < n; i++){
        buf[i] = toupper((unsigned char)buf[i]);
      }
      send(sv[1], buf, n, 0);  // reply on the SAME descriptor we read from
    }
    // recv returned 0: the parent closed its end, so we're done
    close(sv[1]);
    return 0;
  }

  // parent: keep sv[0], close the child's end
  close(sv[1]);
  const char* lines[] = { "hello, child\n", "are you there?\n", "bye\n" };
  for(int i = 0; i < 3; i++){
    send(sv[0], lines[i], strlen(lines[i]), 0);

    // Wait for the reply before sending the next line. This is a stream,
    // so one recv could in principle return part of a reply. These lines
    // are tiny and we take turns, so in practice each comes back whole.
    char buf[256];
    ssize_t n = recv(sv[0], buf, sizeof(buf), 0);
    printf("parent sent: %s", lines[i]);
    printf("child said:  %.*s", (int)n, buf);
  }

  close(sv[0]);   // the child's recv now returns 0
  wait(NULL);
  return 0;
}
