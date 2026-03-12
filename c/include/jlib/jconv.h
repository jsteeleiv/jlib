#ifndef JCONV_H
#define JCONV_H

// This is a basic program written to perform conversions on ...
// binary, decimal, and hexadecimal. Using C language.

#include <jlib/jlib.h>

typedef struct BIN_ARR {
    Error err;
    Logger log;
    char *bin;
    char *buff;
    char **buffs;
    size_t pos;
    size_t cnt;
    size_t size;
} bin_arr;

//int binConv(int b);
char *to_base64(char *data, int size);
int decToBin(int n); //prototyping the only manual function needed ???
BIN_ARR ret_arr(bin_arr input, size_t size);

#ifdef JCONV_IMPL

// sill party trick for swapping without "tmp" var
void xor_swap(int *x, int *y){
    *x^=*y; *y^=*x; *x^=*y;
}

//Decimal to Binary(does not return a value...yet)
int decToBin(int n){
  int b;
	static int binArray[255] = {0};
   for (size_t i = 31; i >= 0; i--){
    if (i == 27 || i == 23 || i == 19 || i == 15 || i == 11 || i == 7 || i == 3)
     printf(" ");   //adding a space every 4 digits to seperate values into nibbles
      b = n >> i;   //shifting bits to the right 31 places to isolate left-most bit for and logic
     if (b & 1)     //then "and"ing each bit against a 1 
       binArray[i] = 1; 
     else
       binArray[i] = 0;     
   } //end for loop
return 0;	 
}

char *to_base64(char *data, int size){
    //assign encoding table {lookup table of allowed chars}
    static const char base64_table[]= 
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "abcdefghijklmnopqrstuvwxyz"
    "0123456789+/";

    char *b64;
    
}

/*typedef struct{
  char bin[32] = {0};
} BIN_ARRAY;

BIN_ARRAY returnArray(BIN_ARRAY array_in, int size){
  BIN_ARRAY returned;

	for(size_t i = 0; i < size; i++){
   returned.bin[i] = array_in.bin[i] + 1;
	 return returned;
	}
}*/

/*int binConv(int b){//returns a value
  int value = b, decValue = 0, base = 1;
	int temp = value;
	 while(temp){
    int lastDigit = temp % 10;
		temp = temp / 10;
		decValue += lastDigit * base;
		base = base * 2;
	 }
	 return decValue;
}*/

#endif /* JCONV_IMPL */
#endif /* JCONV_H */