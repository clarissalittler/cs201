#include "stdio.h"
#include "stdlib.h"

/*
  This is the WORST possible stack you could implement.

  This is only here as a demonstration of pointers to pointers
  
 */

struct Node {
  int v;
  struct Node* next;
};

void pushStack(struct Node** s, int v){
  struct Node* h = malloc(sizeof(struct Node));

  h->v = v;
  h->next = *s;

  *s = h;
}

void popStack(struct Node** s, int* r){
  if(*s == NULL){
    r = NULL;
  }
  else{
    struct Node* temp = (*s)->next;
    *r = (*s)->v;
    free(*s);
    *s = temp;
  }
}

void addressTraverse(struct Node* s){
  if(s != NULL){
    printf("Address of node %p with value %d \n",s,s->v);
    addressTraverse(s->next);
  }
}

int main(){

  struct Node* stackTop = NULL;
  int ret;
  pushStack(&stackTop,1);
  pushStack(&stackTop,2);
  pushStack(&stackTop,3);
  
  addressTraverse(stackTop);
  
  popStack(&stackTop,&ret);
  
  printf("Return value: %d\n", ret);

  addressTraverse(stackTop);
  printf("And now!\n");
  pushStack(&stackTop,4);
  addressTraverse(stackTop);
  
  return 0;
}
