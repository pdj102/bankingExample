 ZX Spectrum Next z88dk banking example
 
 The ZX Spectrum Next physical memory is split into banks
 
 The Next supports two bank sizes
 The classic 16K bank scheme and the Next specific 8K bank scheme
 In the Next scheme each 16k bank is subdivided into two 8k banks
 
 The ZX Spectrum z80 processor can only address 64k. 
 
 For the classic the 16k bank scheme the visible 64K address space is split into 4 * 16K slots
 By default
 slot 1 (0x000)     contains the ROM 
 slot 2 (0x4000)    contains BANK 5 (normally used for the ULA screen)
 slot 3 (0x8000)    contains BANK 2 (free RAM)
 slot 4 (0xc000)    contains BANK 0 (free RAM) 
  
 The classic 128K spectrum style banking scheme can only alter the 16k bank mapped into slot 4 (0xc000)
 Banks 0, 1, 2, 3, 4, 5, 7 can be mapped into slot 4
  
 For the Next 8k banking scheme the visible 64K address spaces is split into 8 * 8k slotes
 By default these slots mirror the classic scheme and the bank numbers are based on the 8k sizes 
 For example 16k slot 2 contains 8k banks 10 and 11
  
 The 8k-bank accessed in an 8k-slot is selected by writing the 8k-bank number to bits 7-0 of the 8 Next registers from Memory management slot 0 bank ($50) upwards 
 $50 addresses 8k-slot 0, $51 addresses 8k-slot 1, and so on.
 
 In addition, in 8k-slots 0 and 1 only, the ROM can be paged in by selecting the otherwise nonexistent 8k-page $FF. 
 Whether the high or the low 8k of the ROM is mapped is determined by which 8k-slot is used. 
  
 To compile C code into a specific bank you need to 
 a) set the ORG address for slot the bank will be paged into within the C code
 b) tell the compiler to compile the code into the correct bank (physical memory location)
 
 To set the org address within a C file use #pragma output CRT_ORG_CODE
 For example #pragma output CRT_ORG_CODE = 0xC000
 This applies to the scope of the file
  
 To compile the code into the correct bank use the --codeseg and --constseg options to specify the bank.
 
 ZXN banks are named as BANK_0 (16k) BANK_0_L (8k) BANK_0_H (8k) BANK_1 (16k) etc
 See https://github.com/z88dk/z88dk/blob/0cb51ee64b02bfc08645026dd141f671c0de6dc9/lib/target/zxn/classic/memory_map.asm
 For example to complile to 16k bank 0 zcc +zxn -vn -c -SO3 -compiler sdcc -clib=sdcc_iy  --codesegBANK_0 --constsegBANK_0 -o ./obj/bank_test_0.o src/bank_test_0.c
  
 By default the stack is set at the top of memory in 16k slot 4
 In the classic 128k banking scheme, if we want to swap a different 16k bank into slot 4 we have to move the stack out of the way 
 Use #pragma output REGISTER_SP to move the stack elsewhere
 It can be moved to the top of bank 5 with #pragma output REGISTER_SP = 0xbfff
 
 To call a function in a bank the bank must be manually paged into the visible 64k memory address space
 The C compiler does not do this automatically for you
 There are banking features in the z88dk compiler but these do not support the ZX Spectrum Next target out of the box?
 Examples of how to page a bank into memory
  
 Classic 128k - page 16k bank 1 into slot 4 (0xc00)
 #include <z80.h>
 z80_outp(0x7ffd, 1);
  
 ZXN - page 8k bank 2 into slot 6 (0xc00)
 ZXN_WRITE_REG(0x56, 2);
 ZXN_WRITE_MMU6(2);
 Remember 8k banks numbers are 2x and 2x+1 the 16k bank number e.g. 16k bank 1 = 8k banks 2 & 3
  
  
 To compile the example
 zcc +zxn -vn -c -SO3 -compiler sdcc -clib=sdcc_iy -pragma-include:zpragma.inc -o bank_test.o bank_test.c
 zcc +zxn -vn -c -SO3 -compiler sdcc -clib=sdcc_iy  --codesegBANK_0 --constsegBANK_0 -o bank_test_0.o bank_test_0.c
 zcc +zxn -vn -c -SO3 -compiler sdcc -clib=sdcc_iy  --codesegBANK_1_L --constsegBANK_1_L -o bank_test_1.o bank_test_1.c
 zcc +zxn -vn -c -SO3 -compiler sdcc -clib=sdcc_iy  --codesegBANK_1 --constsegBANK_1 -o bank_test_1.o bank_test_1.c
 zcc +zxn -vn -clib=sdcc_iy  -startup=1 bank_test.o bank_test_0.o bank_test_1.o -pragma-include:zpragma.inc -o bank_test.nex -create-app -subtype=nex
  
 NB bank 0 is merged into the main memory bank as the main memory bank includes 5, 2, 0 by default
