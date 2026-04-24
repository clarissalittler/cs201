#include <stdio.h>
#include <stdlib.h>
#include <math.h>

/*
 Here is a scaffold for writing the posit program. All you need to do here is fill in definitions for the
 regime, exponent, and mantissa functions.

 This scaffold uses the older/generalized posit<8,3> notation for a small teaching
 example. It is not the 2022 standard posit8 encoding, which uses a two-bit
 exponent field and regime scaling by powers of 16.

 You don't *have* to use this scaffold but you *may*.

 The trick of this small program is that you view the byte as a series of bit positions
 b b b b b b b b
 7 6 5 4 3 2 1 0

 so we pass around the state of where we are in parsing the byte, with position 7 being the sign bit and position 6 being the start of understanding the regime bit.

 *** CONVENTION for the three decode functions below ***
 Each of regime/exponent/mantissa takes an int* p. When called, *p is the index of the
 first bit that function should read (bit 6 for regime, wherever the previous function
 left off for the others). When the function returns, *p must be the index of the first
 *unread* bit, i.e., the bit the next function should start at. If there are no more
 bits, *p ends up at -1.

 This means:
  - regime() reads the run of identical bits AND the terminating bit (if present),
    then leaves *p pointing to the first exponent bit.
  - exponent() reads up to es bits then leaves *p pointing to the first mantissa bit.
  - mantissa() reads whatever's left.

 If you stick to this contract, main() doesn't have to do any index bookkeeping between
 calls — you just chain them.

 As you fill in the functions, get rid of the (void)...
*/

const int es = 3;

int getBit(unsigned char n, int p){
  if (p < 0 || p > 7) return 0;   // defensive: keep us out of UB territory
  return ((n >> p) & 1);
}

// setBitOn is a helper you'll probably want inside exponent() for assembling
// the bit-string you read into an integer value. Not called anywhere in this
// scaffold — it's here for you to use.
void setBitOn(int* n, int b, int p){
  int shifty = b << p;
  *n = *n | shifty;
}

int regime(unsigned char n, int* p){
  (void)n;
  (void)p;
  // Start at the bit indicated by *p (typically bit 6).
  // Count the run of identical bits; stop when you hit a different bit
  // (the "terminator") or fall off the end of the byte.
  // Advance *p past every bit you consume, including the terminator.
  // Return k: if run started with 0, k = -m; if with 1, k = m - 1.

}

int exponent(unsigned char n, int* p){
  (void)n;
  (void)p;
  // Read up to es bits starting at *p, left-to-right (highest bit first).
  // If fewer than es bits remain, the bits you have are the *most significant*
  // bits of the exponent field — pad the missing low bits with zero (i.e.,
  // left-shift by (es - bits_read) at the end).
  // Advance *p past every bit you consume.

}

float mantissa(unsigned char n, int* p){
  (void)n;
  (void)p;
  // Read every remaining bit starting at *p. The bits form the fractional
  // part f, with the first bit worth 2^-1, the next 2^-2, etc.
  // Return 1 + f (the implicit leading 1 is baked in).

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
  return n == 0;
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

      int k = regime(decoded, &place);
      int e = exponent(decoded, &place);
      float m = mantissa(decoded, &place);
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
