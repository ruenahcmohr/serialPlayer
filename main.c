/***********************************************************************************

Inspried by PHPPLD.
2021 Rue Mohr.
This generic ROM-generator framework was customized for:

scanless keyboard encoder LUT

This is set up for a 9x9 grid, with row0 and column 0 not present, a secondary system needs 
  to be used to know that a key has been pressed, and there isn't complete detection of 
  a keyboard-mash situation (but it tries)

INPUTS
 

OUTPUTS
 
FEEDBACK
 N/A



Address bits      8 bit rom size

       -- no parallel roms available --
     8                  2 k
     9                  4 k
     10                 8 k
     
       -- eeproms available from here --
     11                 16 k  (28C16)
     12                 32 k  (28C32)
     
       -- eprom practical sizes from here --
     13                 64 k  (2764)
     14                 128 k (27128)
     15                 256 k 
     16                 512 k
     17                 1 M  (27010)
     18                 2 M
     19                 4 M
     20                 8 M

       -- flash from here up --



**************************************************************************************/


#include <stdio.h>
#include <stdint.h>
#include "ROMLib.h"


// the number of address lines you need !!!???!!!
#define InputBits 7

// the output data size, 8 or 16
#define OutputBits 8

// default output value
#define DFOutput  0xFE



// Tuck this one away!. Bit reverser!  Please dont use this in real fft code,
//   YOU KNOW how many bits your working with, and you can use a 
//   specific case generator for it.
uint8_t uniReverse(uint8_t i, uint8_t bits) {

  uint8_t r, m, b;
  r = 0;             // result
  m = 1 << (bits-1); // mask will travel right
  b = 1;             // bit will travel left
  
  while(m) {
    if (i&b) r |=m;
    b <<= 1;
    m >>= 1;  
  }
  
  return r;  

}


// count set bits, unrolled edition.
// if using assember shift into the carry and use addc, 0
uint8_t bitCount(uint16_t n) {  
   uint8_t rv;
   rv = 0;
   if (n & 0x0001) rv++;
   if (n & 0x0002) rv++;
   if (n & 0x0004) rv++;
   if (n & 0x0008) rv++;
   if (n & 0x0010) rv++;
   if (n & 0x0020) rv++;
   if (n & 0x0040) rv++;
   if (n & 0x0080) rv++;   
   if (n & 0x0100) rv++;
   if (n & 0x0200) rv++;
   if (n & 0x0400) rv++;
   if (n & 0x0800) rv++;
   if (n & 0x1000) rv++;
   if (n & 0x2000) rv++;
   if (n & 0x4000) rv++;
   if (n & 0x8000) rv++; 
   
   return rv;
}

// convert a character array and bit count to a serial stream, 10 bits per character. 
uint8_t SerialMessage(char * message, uint16_t bit) {
  uint16_t index;
  uint8_t  subBit;
  
  index  = bit/10;
  subBit = bit%10;
  
 // printf("I:%d, b:%d\n", index, subBit);
  
  switch (subBit) {
    case 0: return 0; // start bit
    case 1: return (message[index] & 0x01)!=0;
    case 2: return (message[index] & 0x02)!=0;
    case 3: return (message[index] & 0x04)!=0;
    case 4: return (message[index] & 0x08)!=0;
    case 5: return (message[index] & 0x10)!=0;
    case 6: return (message[index] & 0x20)!=0;
    case 7: return (message[index] & 0x40)!=0;
    case 8: return (message[index] & 0x80)!=0;
    case 9: return 1; // stop bit          
  }

}



int main(void) {

  uint16_t bitcI;
  uint8_t  TxDO; 


  uint32_t out;  // leave it alone!

  setup();       // open output file.
  
  
  // loop thru each address
  for( A=0; A<(1<<InputBits); A++) { // A is a bitfield (a mash-up of the vars we want)
       
     // reset vars  
     bitcI = 0;
     // build input values
    spliceValueFromField( &bitcI,            A,  7,   0,1,2,3,4,5,6);        // 7 bit count

     // do task
     if (bitcI < 120) { // 12 characters, 120 bits.
       TxDO = SerialMessage("No Computer\n", bitcI);
     } else {
       TxDO = 1;
     }
   
     // reconstitute the output
     // assign default values for outputs     
     out = DFOutput;
     spliceFieldFromValue( &out, TxDO,     1,  0); 
    
 //    spliceFieldFromValue( &out, multKeyO,  1,  7);           // add to output, multiKeyO, which is 1 bit, placed at bit 7 of output     
  //   spliceFieldFromValue( &out, keyO,      1,  7);           // add to output, keyO, which is 1 bit, placed at bit 7 of the output
          
     // submit entry to file
     write(fd, &out, OutputBits>>3);  // >>3 converts to bytes, leave it!
  }
  
  cleanup(); // close file
  
  return 0;
}








