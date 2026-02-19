#include <stdio.h>
#include <stdlib.h>
#include <math.h>

/*
 Here is a scaffold for writing the posit program. All you need to do here is fill in definitions for the
 regime, exponent, and mantissa functions.

 You don't *have* to use this scaffold but you *may*.

 The trick of this small program is that you view the byte as a series of bit positions
 b b b b b b b b
 7 6 5 4 3 2 1 0

 so we pass around the state of where we are in parsing the byte, with position 7 being the sign bit and position 6 being the start of understanding the regime bit.

 If you're clever with how you use the pointer argument this should be a pretty short program!
*/

int es = 3;

int getBit(unsigned char n,int p){
  return ((n >> p) & 1);
}

int regime(unsigned char n, int* p){
  // always start at the place indicated by the second argument
  // this is going to be the trick we use for the entire program

}

void setBitOn(int* n, int b, int p){
  int shifty = b << p;
  *n = *n | shifty;
}

int exponent(unsigned char n, int* p){

}

float mantissa(unsigned char n, int* p){

}

void flipBit(unsigned char* n, int p){
  int shifty = 1 << p;
  *n = *n ^ shifty;
}

void showBits(unsigned char n){
  for(int i =7; i>=0; i--){
    printf("%d",getBit(n,i));
  }
  printf("\n");
}

int checkNaR(unsigned char n){
  return n == 0x80;
}

int checkZero(unsigned char n){
  int isZero = 1;

  for(int i =0; i < 8; i++){
    isZero = isZero & !getBit(n,i);
  }
  
  return isZero;
  
}


int main(){

  unsigned char num = 0;
  float posited = 0;
  int place;
  int temp;

  
  while(1){
    place = 6;
    printf("Your number as bits is: ");
    showBits(num);

    
    printf("And your number as posit is:  ");
    if(checkNaR(num)){
      printf("Nar!\n");
    }
    else if(checkZero(num)){
      printf("0!\n");
    }
    else {
      int sign = getBit(num, 7);
      unsigned char decoded = num;
      if (sign) {
        decoded = (~num) + 1;
      }

      int k = regime(decoded,&place);
      place--;
      int e = exponent(decoded,&place);
      float m = mantissa(decoded,&place);
      printf("Regime k=%d\n",k);
      printf("Exponent e=%d\n",e);
      printf("Mantissa m=%f\n",m);
      posited = powf(256.0f, k) * powf(2.0f, e) * m;
      if (sign) {
        posited = -posited;
      }

      printf("%f\n",posited);
    }

    
    printf("Enter a place to flip a bit: ");
    scanf("%d",&temp);
    flipBit(&num,temp);
    
  }
  
  return 0;
}
