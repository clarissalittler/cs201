#include <stdio.h>

int badder(int n1, int n2){
  return (n1 + n2 - 1);
}

int foldl(int arr[],int l, int (*op)(int,int)){
  int res = 0;
  for(int i = 0; i < l;i++){
    res = op(res,arr[i]);
  }

  return res;
}

int main(){

  int arr[5] = {0,1,2,3,4};

  printf("Our silly fold returned %d\n",foldl(arr,5,badder));
  
  return 0;
}
