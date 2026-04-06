#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
//#include <jlib/jlib.h>


//prototyping color functions
void clrWhite();
void clrRed();
void clrBlue();
void clrCyan();
void clrYellow(int c);
void clrGreen();
void clrPurp();
void clrReset();//resets to default, need to use after each change is finished printing


//start of program execution
int main(){
//int run = 1;

/*do{*/
  int choice = 0;
	clrYellow(1);//change printed color to yellow
  printf("\n          ***Convert10 is your Conversion Friend!***\n\n");
  clrReset();//revert to default color
  printf("0. Quit / 1. Decimal / 2. HexaDecimal / 3. Octal / 4. Binary\n\nPlease choose:  ");
  scanf("%d", &choice);
  
  while(choice != 0){

   if(choice == 1){ int d; /*int *p*/;
  	printf("Decimal:     "); scanf("%d", &d);//user prompt and input
  	if(d == 0){break;}
  	else {
		printf("  Octal:   0o%o\n", d); printf("Heximal:   0x%x\n", d); printf(" Binary:   0b");
  	decToBin(d); 
	//	for(size_t i = 0; i < 32; i++) {
	//	 printf("%d", *(p + i));}}
   }}//end of if 1
   if(choice == 2){char s[33]; int x;
    printf("Heximal:   0x"); scanf("%s", s); sscanf(s, "%x", &x);
		if(x == 0){break;}
  	printf("Decimal:     %u\n", x); printf("  Octal:   0o%o\n", x); printf(" Binary:   0b");
  	decToBin(x); printf("\n");
   }//end of if 2
	 if(choice == 3){int o;
  	printf("  Octal:  0o"); scanf("%o", &o);
		if(o == 0){break;}
  	printf("Decimal:    %d\n", o); printf("Heximal:  0x%x\n", o); printf("Binary :  0b");
  	decToBin(o); printf("\n");
   }//end of if 3
	 if(choice == 4){int b;
    printf(" Binary:  0b"); scanf("%d", &b);
		if(b == 0){break;}
	//	printf("Decimal:    %d\n", binConv(b)); printf("Heximal:  0x%x\n", binConv(b));
  //	printf("  Octal:  0o%o\n", binConv(b)); printf("\n");
	 }//end of if 4
 }//end of inner while loop (choice)
/* printf("Would you like to continue?\n0 = No / 1 = Yes "); scanf("%d", &run);
}while(run == 1);//end of outer do while loop (run)*/
return 0; 
}//end of main function

//defining color functions
void clrRed(){printf("\033[0;31m");}
void clrBlue(){printf("\033[0;34m");}
void clrCyan(){printf("\033[0;36m");}
void clrYellow(int c){
  if(c == 1)printf("\033[1;33m");
	else printf("\033[0;33m");}
void clrGreen(){printf("\033[0;32m");}
void clrPurp(){printf("\033[0;35m");}
void clrWhite(){printf("\033[0;37m");}
void clrReset(){printf("\033[0;0m");}