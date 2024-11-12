#include <stdio.h> // Includes the standard input/output library, allowing us to use functions like printf for printing to the console.
#include <stdlib.h> // Includes the standard library, providing general-purpose functions like memory allocation (malloc).

const int thing = 10; // Declares a constant integer variable named 'thing' and initializes it with the value 10. 'const' ensures the value cannot be changed later in the program.

#define thing2 20 // Defines a macro named 'thing2' which will be replaced by 20 wherever it appears in the code. Macros are processed by the preprocessor before compilation.

struct point { // Defines a structure named 'point' to represent a point in a 2D space.
  double x; // Represents the x-coordinate of the point, a double-precision floating-point number.
  double y; // Represents the y-coordinate of the point, also a double-precision floating-point number.
};

const struct point p = {20, 30}; // Declares a constant structure variable named 'p' of type 'point' and initializes its members with the values 20 and 30.

// #define p2 = {20,30} // This line is commented out because it is incorrect syntax. Macro definitions cannot be used to initialize structures.

#define msg "Hi there" // Defines a macro named 'msg' which will be replaced by the string "Hi there" wherever it appears in the code.

/*
int myMax(int x, int y){
  return (x > y ? x : y);
}
*/

#define myMax(x,y) (x > y ? x : y) // Defines a macro named 'myMax' which will be replaced by the expression (x > y ? x : y) wherever it appears in the code. This macro effectively implements a function-like behavior to find the maximum of two values. 

#define badSizeof(x) 1+sizeof(x) // Defines a macro named 'badSizeof' which will be replaced by the expression 1+sizeof(x) wherever it appears in the code. This macro incorrectly attempts to calculate the size of an expression.

#define size2(x) sizeof(typeof(x)) // Defines a macro named 'size2' which will be replaced by the expression sizeof(typeof(x)) wherever it appears in the code. This macro correctly calculates the size of the data type of the expression 'x'.

int main(){

  //thing = 0xFFFFFFFF; // This line is commented out because it would attempt to modify the value of the constant variable 'thing', which is not allowed.

  //printf("Here's a thing: %d\n",myMax("10",20)); // This line is commented out because it would lead to undefined behavior. The macro 'myMax' expects integer values as arguments, but "10" is a string literal.

  printf("Here's a thing: %d\n",size2(5.0)); // This line prints the size of a double-precision floating-point number, which is typically 8 bytes. 

  return 0; // Indicates that the program executed successfully.
}
