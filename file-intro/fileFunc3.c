#include <stdio.h>
#include <stdlib.h>

/*
 let's talk about fprintf and fscanf
 fprintf(FILE*,char*,...)

 fscanf(FILE*,char*,...) // remember to put pointers
*/

struct petData {
  char name[50];
  char species[50];
  int age;
};

void printPet(FILE* f,struct petData p){
  fprintf(f,"%s,%s,%d\n",p.name,p.species,p.age);
}

void readPet(FILE* f, struct petData* p){
  fscanf(f,"%49[^,],%49[^,],%d",p->name,p->species,&p->age);
}

int main(){

  FILE* petFile = fopen("pets.txt","r");
  FILE* petFile2 = fopen("pets2.txt","w");
  
  struct petData p;

  readPet(petFile,&p);

  printf("%s is a %s and is %d years old\n",p.name,p.species,p.age);
  
  fclose(petFile);
  fclose(petFile2);
  
  return 0;
}
