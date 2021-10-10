# ZX Spectrum Next z88dk banking example
 
The ZX Spectrum z80 processor can only directly address 64k of memory but the ZX Spectrum 128 and ZX Spectrun Next models are equipped with far more physical memory than this. Bank switching is used to enable this extra memory to be directly addressed by the processor. 
 
The ZX Spectrum Next physical memory is split into banks that can be mapped into the directly addressable memory by the Memory Management Unit (MMU). 
 
The Next supports two bank sizes, the classic 16K bank scheme and the Next specific 8K bank scheme. In the Next scheme each classic 16k bank is subdivided into two 8k banks.

## Classic 16k banking scheme

The spectrum 128 classic banking scheme divides the visible 64K address space into 4 * 16K slots. By default banks are mapped into the slots as follows
 * slot 1 (0x000)     contains the ROM 
 * slot 2 (0x4000)    contains BANK 5 (normally used for the ULA screen)
 * slot 3 (0x8000)    contains BANK 2 (free RAM)
 * slot 4 (0xc000)    contains BANK 0 (free RAM) 
 
In the classic 128K spectrum style banking scheme the MMU can only alter the 16k bank mapped into slot 4 (0xc000). Banks 0, 1, 2, 3, 4, 5, 7 can be mapped into slot 4.

## Next 8k banking scheme

The Next 8k banking scheme divides the addresable 64K address space into 8 * 8k slots. 

The Next banking 8k scheme is much more flexible than the classic scheme and can map any bank into any 8k slot. It also supports much bigger physcial memory sizes ie. 1 or 2Mb instead of 128K.  

The Next 8k bank numbers are double the classic bank numbers e.g. 8k banks 10 and 11 refer to the same memory as bank 2 in the classic 16k scheme. 

The 8k-bank accessed in an 8k-slot is selected by writing the 8k-bank number to bits 7-0 of the 8 Next registers from Memory management slot 0 bank ($50) upwards. $50 addresses 8k-slot 0, $51 addresses 8k-slot 1, and so on.
 
In addition, in 8k-slots 0 and 1 only, the ROM can be paged in by selecting the otherwise nonexistent 8k-page $FF. Whether the high or the low 8k of the ROM is mapped is determined by which 8k-slot is used. 
  
## Placing C code in a specific bank

To place C code into a specific bank you need to 
* set the ORG address for slot the bank will be paged into 
* tell the compiler to compile the code into the correct bank (physical memory location)
* move the stack out of harms way - ensure the stack is not located in memory to be banked out 
* manually map the bank into memory before calling the C code in the bank 

### Setting the org address

To set the org address within a C file use #pragma output CRT_ORG_CODE, example #pragma output CRT_ORG_CODE = 0xC000.This pragma applies to the scope of the file.

### Compiling 

To compile the code into the correct bank use the --codeseg and --constseg options to specify the bank.
 
ZXN banks are named as BANK_0 (16k) BANK_0_L (8k) BANK_0_H (8k) BANK_1 (16k) etc.

See https://github.com/z88dk/z88dk/blob/0cb51ee64b02bfc08645026dd141f671c0de6dc9/lib/target/zxn/classic/memory_map.asm

For example to complile to 16k bank 0 zcc +zxn -vn -c -SO3 -compiler sdcc -clib=sdcc_iy  --codesegBANK_0 --constsegBANK_0 -o ./obj/bank_test_0.o src/bank_test_0.c
  
### Move the stack out of harms way

By default the stack is set at the top of memory in 16k slot 4.

In the classic 128k banking scheme, if we want to swap a different 16k bank into slot 4 we have to move the stack out of the way.

Use #pragma output REGISTER_SP to move the stack elsewhere.

It can be moved to the top of bank 5 with #pragma output REGISTER_SP = 0xbfff
 
### Manually maping a bank into memory

To call a C function compiled into a bank the bank must be manually paged into the visible 64k memory address space first.

The C compiler does not do this automatically for you!
 
There are banking features in the z88dk compiler that automate some of this process for you but these do not seem to support the ZX Spectrum Next target out of the box at the time of trying(?). So we will map the bank the into memory manually. 

Examples of how to page a bank into memory.
  
Classic 128k - page 16k bank 1 into slot 4 (0xc00)
 
#include <z80.h>

z80_outp(0x7ffd, 1);
 

ZXN - page 8k bank 2 into slot 6 (0xc00)
 
ZXN_WRITE_REG(0x56, 2); or
 
ZXN_WRITE_MMU6(2);
 
 
## How to compile the example code 

To compile the example

zcc +zxn -vn -c -SO3 -compiler sdcc -clib=sdcc_iy -pragma-include:zpragma.inc -o bank_test.o bank_test.c
 
zcc +zxn -vn -c -SO3 -compiler sdcc -clib=sdcc_iy  --codesegBANK_0 --constsegBANK_0 -o bank_test_0.o bank_test_0.c
 
zcc +zxn -vn -c -SO3 -compiler sdcc -clib=sdcc_iy  --codesegBANK_1_L --constsegBANK_1_L -o bank_test_1.o bank_test_1.c
 
zcc +zxn -vn -c -SO3 -compiler sdcc -clib=sdcc_iy  --codesegBANK_1 --constsegBANK_1 -o bank_test_1.o bank_test_1.c
 
zcc +zxn -vn -clib=sdcc_iy  -startup=1 bank_test.o bank_test_0.o bank_test_1.o -pragma-include:zpragma.inc -o bank_test.nex -create-app -subtype=nex
 
  
NB bank 0 is merged into the main memory bank as the main memory bank includes 5, 2, 0 by default
