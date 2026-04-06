#include <stdio.h>

void arrayMunger(int a[], int s){
  for(int i=0; i<s; i++){
    a[i] = 2*a[i];
  } 
}

void arrayPrinter(int a[], int s){
  for(int i=0; i<s; i++){
    // not cout << a[i] << endl;
    printf("%d\n",a[i]);
  }
}

int main(){

  int arr[10] = {0,1,2,3,4,5,6,7,8,9};

  // arr[i] ===> *(arr + i)
  
  arrayPrinter(arr,10);
  arrayMunger(arr,10);
  arrayPrinter(arr,10);
  
  return 0;
}
