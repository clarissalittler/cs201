#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[]){

  // Check if filename was provided
  if (argc < 2) {
    fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
    return 1;
  }

  // Open file and check for errors
  FILE *ourFile = fopen(argv[1], "r");
  if (ourFile == NULL) {
    perror("Error opening file");
    return 1;
  }

  char line[2048];

  // Read and print file contents
  char* ptr = fgets(line, sizeof(line), ourFile);
  while (ptr != NULL){
    printf("%s", line);
    ptr = fgets(line, sizeof(line), ourFile);
  }

  // Close the file
  fclose(ourFile);

  return 0;
}
