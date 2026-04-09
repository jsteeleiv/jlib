#pragma once
#ifndef JCONV_H
#define JCONV_H

// This is a basic program written to perform conversions on ...
// binary, decimal, and hexadecimal. Using C language.

#include <png.h>
#include <stdio.h>
#include <stdlib.h>
#include <setjmp.h>
#include <jpeglib.h>

#include "../jstd.h"
#include "../jtype.h"

typedef struct BIN_ARR {
    Jerror err;
    Jlog log;
    char *bin;
    char *buff;
    char **buffs;
    size_t pos;
    size_t cnt;
    size_t size;
} binarray_t;

//int binConv(int b);
char *to_base64(char *data, int size);
int decToBin(int n); //prototyping the only manual function needed ???
binarray_t ret_arr(binarray_t input, size_t size);

#ifdef JCONV_IMPL

int test() {
    const char* pngFileName = "input.png";
    const char* jpgFileName = "output.jpg";
    
    convertPNGtoJPEG(pngFileName, jpgFileName);
    
    printf("Conversion from PNG to JPEG complete.\n");
    
    return 0;
}

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

// Function to convert PNG to JPEG
void convertPNGtoJPEG(const char* pngFileName, const char* jpgFileName) {
    FILE *infile = fopen(pngFileName, "rb");
    if (!infile) {
        fprintf(stderr, "Error opening PNG file: %s\n", pngFileName);
        exit(1);
    }

    // Initialize the PNG reader
    png_structp png = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png) {
        fprintf(stderr, "Error initializing PNG reader\n");
        fclose(infile);
        exit(1);
    }

    png_infop info = png_create_info_struct(png);
    if (!info) {
        fprintf(stderr, "Error initializing PNG info\n");
        png_destroy_read_struct(&png, NULL, NULL);
        fclose(infile);
        exit(1);
    }

    png_bytep row_pointers[3];
    int width, height, color_type;
    png_read_png(png, info, PNG_TRANSFORM_EXPAND, NULL);

    png_get_IHDR(png, info, &width, &height, &color_type, NULL, NULL, NULL, NULL);

    // Initialize the JPEG writer
    struct jpeg_compress_struct cinfo;
    struct jpeg_error_mgr jerr;
    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_compress(&cinfo);
    
    FILE *outfile = fopen(jpgFileName, "wb");
    if (!outfile) {
        fprintf(stderr, "Error creating JPEG file: %s\n", jpgFileName);
        png_destroy_read_struct(&png, &info, NULL);
        exit(1);
    }

    jpeg_stdio_dest(&cinfo, outfile);
    cinfo.image_width = width;
    cinfo.image_height = height;
    cinfo.input_components = 3;
    cinfo.in_color_space = JCS_RGB;
    jpeg_set_defaults(&cinfo);
    jpeg_start_compress(&cinfo, TRUE);

    row_pointers[0] = (png_bytep) malloc(3 * width * sizeof(png_byte));
    row_pointers[1] = (png_bytep) malloc(3 * width * sizeof(png_byte));
    row_pointers[2] = (png_bytep) malloc(3 * width * sizeof(png_byte));

    while (height--) {
        png_bytep row = png_get_rows(png, info)[height];
        for (int x = 0; x < width; x++) {
            png_bytep px = &(row[x * 4]);
            row_pointers[0][x * 3 + 0] = px[0];
            row_pointers[0][x * 3 + 1] = px[1];
            row_pointers[0][x * 3 + 2] = px[2];
        }
        jpeg_write_scanlines(&cinfo, row_pointers, 1);
    }

    jpeg_finish_compress(&cinfo);
    jpeg_destroy_compress(&cinfo);
    fclose(outfile);

    png_destroy_read_struct(&png, &info, NULL);
    fclose(infile);
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