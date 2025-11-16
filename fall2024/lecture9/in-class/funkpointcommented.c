#include <stdio.h>

// This function takes two integers as input and returns their sum minus 1.
// This function is used as the "operation" for the foldl function.
int badder(int n1, int n2){
  return (n1 + n2 - 1);
}

// This function implements a left fold (foldl) operation.
// It takes an array of integers, the length of the array, and a pointer to a function as input.
// The function pointer represents the operation to be performed on each element of the array.
// The foldl function iterates through the array, applying the given operation to the current element and the result of the previous operation.
int foldl(int arr[],int l, int (*op)(int,int)){
  // Initialize the result to 0.
  int res = 0;
  // Iterate through the array.
  for(int i = 0; i < l;i++){
    // Apply the operation to the current element and the previous result.
    res = op(res,arr[i]);
  }

  // Return the final result.
  return res;
}

// Main function.
int main(){

  // Declare and initialize an array of integers.
  int arr[5] = {0,1,2,3,4};

  // Call the foldl function with the array, its length, and the badder function as arguments.
  // The result of the foldl function is printed to the console.
  printf("Our silly fold returned %d\n",foldl(arr,5,badder));
  
  // Return 0 to indicate successful execution.
  return 0;
}
