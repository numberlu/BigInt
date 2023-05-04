#ifndef FUNCTIONS_H_INCLUDED
#define FUNCTIONS_H_INCLUDED

#include <stdio.h>

typedef struct BigInt{
    int size; // number of digits in the integer
    char sign;
    int *digits; // array of digits, stored in reverse order (e.g. "123" is stored as [3, 2, 1])
} BigInt;

BigInt readBigInt(FILE *input);
void printBigInt(BigInt bint);
int compareAbsoluteValues(BigInt bi1, BigInt bi2);
BigInt plus(BigInt bi1, BigInt bi2, BigInt result, int max_size);
BigInt minus(BigInt bi1, BigInt bi2, BigInt result, int max_size, int conv);
BigInt add(BigInt bi1, BigInt bi2);
BigInt sub(BigInt bi1, BigInt bi2);
BigInt mul(BigInt bi1, BigInt bi2);
BigInt divide(BigInt bi1, BigInt bi2);

#endif