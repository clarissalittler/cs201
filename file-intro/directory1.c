#include <stdio.h>
#include <dirent.h>
#include <errno.h>
#include <string.h>

void listDir(const char *path) {
    DIR *dir = opendir(path);
    if (dir == NULL) {
        printf("Error opening directory: %s\n", strerror(errno));
        return;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
      if(entry->d_type == DT_REG){
        printf("%s\n", entry->d_name);
      }
    }

    closedir(dir);
}

int main(int argc, char* argv[]){

  listDir(argv[1]);
  
  return 0;
}
