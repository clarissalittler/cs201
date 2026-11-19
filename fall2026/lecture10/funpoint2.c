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

int multer(int n, int m){
  return m*n;
}

void maparr(int arr[], int s,
	    int (*func)(int, int)){
  // this third parameter says
  // "pass in a function that takes an int and returns an int", we're going to call that parameter "func"
  for(int i=0; i<s; i++){
    arr[i] = func(arr[i],3);
  }
}

int main(){

  int arr[5] = {0,1,2,3,4};

  maparr(arr,5,multer);

  for(int i=0; i<5; i++){
    printf("%d\n",arr[i]);
  }
  
  return 0;
}

