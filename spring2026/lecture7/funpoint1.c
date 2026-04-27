#include <stdio.h>


// the point of function pointers
// is that they give you a way to
// pass function as first-class argument
// if you've used many modern languages
// and some ancient ones, you'll have seen
// such "higher-order functions" before
// that take other functions as arguments
//
// programs that /rely/ on this style
// are often called "functional programs"/
// "functional programming"
// in this file, we'll write a small program
// with two functions in it
// the first function doubles a number
// the second function "maps" a function as argument into an array, applying the function at every point in the array

int doubler(int n){
  return 2*n;
}

void maparr(int arr[], int s,
	    int (*func)(int)){
  for(int i=0; i<5; i++){
    arr[i] = func(arr[i]);
  }
}

int main(){

  int arr[5] = {0,1,2,3,4};

  maparr(arr,5,doubler);

  for(int i=0; i<5; i++){
    printf("%d\n",arr[i]);
  }
  
  return 0;
}

