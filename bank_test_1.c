#include <arch/zxn.h>           // ZX Spectrum Next architecture specfic functions
#include <stdint.h>             // standard names for ints with no ambiguity 
#include <input.h>
#include <stdio.h>

#include "bank_test.h"

#pragma output CRT_ORG_CODE = 0xC000

void test_bank1(uint8_t v)
{
    printf("Printing from bank 1 - passed value %d\n",v);

} 