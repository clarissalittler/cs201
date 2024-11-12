#include <stdio.h> // Include the standard input/output library for functions like printf

// Define a structure called "point" to represent a point in 2D space
struct point {
  double x;  // The x-coordinate of the point (a double-precision floating-point number)
  double y;  // The y-coordinate of the point (a double-precision floating-point number)
};

// Function to calculate the square of the magnitude (length) of a point
double magSq(struct point p){ 
  return (p.x*p.x + p.y*p.y); // Calculate the square of the magnitude using the Pythagorean theorem
}

int main(){

  // Create a variable of type "point" named "p1"
  struct point p1;
  p1.x = 5.0;  // Set the x-coordinate of p1 to 5.0
  p1.y = 10;  // Set the y-coordinate of p1 to 10

  // Create a pointer to a double called "dubstep" and make it point to the memory location of p1
  double* dubstep = &p1; 
  // Notice that "p1" is treated as a pointer to the first element of the structure which is p1.x
  printf("This is (hopefully) p1.x %f\n",*dubstep); // Print the value of the x-coordinate of p1, which is accessed using the pointer "dubstep"
  printf("This is (hopefully) p1.y %f\n",*(dubstep + 1)); // Print the value of the y-coordinate of p1, by incrementing the pointer "dubstep" to access the next element of the structure

  printf("The d^2 of this vector is %f\n",magSq(p1)); // Calculate and print the square of the magnitude of p1 using the magSq function

  return 0; // Indicate successful execution of the program
}
