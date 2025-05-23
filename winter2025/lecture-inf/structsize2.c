#include <stdio.h>
#include <stdlib.h>

struct exA {
	int int_field[3]; // 12 bytes = 29 bytes
	char char_field; // 1 byte
	long array[2];   // 16 bytes
};

struct exB {
	char char_field; // 1 byte
	long array[2];   // 16 bytes
	int int_field[3];   // 12 bytes = 29 bytes
};

struct exC {
	long array[2];   // 16 bytes 
	int int_field[3];   // 12 bytes 
    char char_field; // 1 byte = 29 bytes
};

int main()
{
   struct exA one;
   struct exB two;
   struct exC three;
   int size1 = sizeof(one);
   int size2 = sizeof(two);
   int size3 = sizeof(three);
   printf("exA: %d  exB: %d  exC: %d\n", size1, size2, size3);   
   return 0;
}
