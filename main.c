#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "main.h"

BigInt readBigInt(FILE *input) {
    BigInt bint;
    char input_str[1000];
    fgets(input_str, sizeof(input_str), input);

    input_str[strcspn(input_str, "\n")] = 0; // Remove the newline character at the end of input

    //Test if the integer is negative
    if(input_str[0] == '-')
    {
        bint.sign = '-';
        bint.size = strlen(input_str) - 1;
        for(int i = 0; i < bint.size; ++i)
        {
            input_str[i] = input_str[i+1];
        }
    }
    else{
        bint.sign = '\0';
        bint.size = strlen(input_str);
    }

    bint.digits = (int*)malloc(bint.size * sizeof(int));

    for (int i = 0; i < bint.size; i++) {
        if(isdigit(input_str[i]))
        {
            bint.digits[i] = input_str[bint.size - i - 1] - '0'; // Convert the character to an integer and store in reverse order
        }
        else{
            bint.size = i + 1;
            bint.digits = (int*)realloc(bint.digits, bint.size * sizeof(int));
            break;
        }
    }

    return bint;
}

void printBigInt(BigInt bint)
{
    printf("Digits array: ");
    printf("%c ", bint.sign);
    for (int i = bint.size - 1; i >= 0; i--) {
        printf("%d ", bint.digits[i]);
    }
    printf("\n");
}

int compareAbsoluteValues(BigInt bi1, BigInt bi2) //returns 0 if the absolute value of bi1 is larger than bi2
{
    if(bi1.size > bi2.size) return 0;
    else if(bi1.size < bi2.size) return 1;
    else{
        for(int i = bi1.size - 1; i >= 0; --i)
        {
            int n = bi1.digits[i] - bi2.digits[i];
            if(n > 0) return 0;
            else if(n < 0) return 1;
        }
    }
}

BigInt plus(BigInt bi1, BigInt bi2, BigInt result, int max_size)
{
    int carry = 0;
    for (int i = 0; i < max_size; ++i){
        int sum = 0;
        if(i < bi1.size && i < bi2.size) 
        {
            sum = bi1.digits[i] + bi2.digits[i] + carry;
        }
        else if(i < bi1.size && i >= bi2.size)
        {
            sum = bi1.digits[i] + carry;
        }
        else if(i >= bi1.size && i < bi2.size)
        {
            sum = bi2.digits[i] + carry;
        }
        carry = sum / 10;
        result.digits[i] = sum % 10;

        if(i == max_size - 1 && carry == 1)  // add any final carry to the last digit
        {
            result.digits[max_size] = sum / 10;
        }
        else if (i == max_size - 1 && carry == 0) //remove leading zeros
        {
            max_size--;
            do{
                result.size--;
                max_size--;
            }
            while(result.digits[max_size] == 0 && max_size != 0);
        }
    }

    return result;
}

BigInt minus(BigInt bi1, BigInt bi2, BigInt result, int max_size, int conv){
    int carry = 0;
    for (int i = 0; i < max_size; ++i){
        int sum = 0;
        if(i < bi1.size && i < bi2.size){
            if((bi1.digits[i] < (bi2.digits[i] + carry)) && i != max_size - 1)
            {
                sum = (bi1.digits[i] + 10) - bi2.digits[i] - carry;
                carry = 1;
            }
            else if((bi1.digits[i] >= (bi2.digits[i] + carry)) && i != max_size - 1)
            {
                sum = bi1.digits[i] - bi2.digits[i] - carry;
                carry = 0;
            }
            else if((bi1.digits[i] < bi2.digits[i] + carry) && i == max_size - 1)
            {
                sum = bi1.digits[i] + 10 - bi2.digits[i] - carry;
                carry = 1;
            }
            else{
                sum = bi1.digits[i] - bi2.digits[i] - carry;
                carry = 0;
            }
        }
        else if(i < bi1.size && i >= bi2.size){
            if(bi1.digits[i] < carry && i != max_size - 1)
            {
                sum = (bi1.digits[i] + 10) - carry;
                carry = 1;
            }
            else if(bi1.digits[i] >= carry && i != max_size - 1)
            {
                sum = bi1.digits[i] - carry;
                carry = 0;
            }
            else if(bi1.digits[i] < carry && i == max_size - 1)
            {
                sum = bi1.digits[i] + 10 - carry;
                carry = 1;
            }
            else{
                sum = bi1.digits[i] - carry;
                carry = 0;
            }
        }

        result.digits[i] = sum;

        //remove leading zeros
        if (i == max_size - 1 && result.digits[max_size - 1] == 0 && carry == 0 && max_size != 1) { 
            max_size--;
            do{
                result.size--;
                max_size--;
            }
            while(result.digits[max_size] == 0 && max_size != 0);
        }
        
    }

    return result;
}

BigInt sub(BigInt bi1, BigInt bi2) {
    int conv = 0;
    int max_size = (bi1.size > bi2.size) ? bi1.size : bi2.size;
    //BigInt* result = (BigInt*)malloc(sizeof(BigInt)); // allocate space for the result BigInt
    BigInt result;
    result.size = ((bi1.sign != '-' && bi2.sign == '-') || (bi1.sign == '-' && bi2.sign != '-')) ? max_size + 1 : max_size; 
    result.digits = (int*)malloc(result.size * sizeof(int));
    if(((bi1.sign != '-' && bi2.sign != '-') || (bi1.sign == '-' && bi2.sign == '-')) && (compareAbsoluteValues(bi1, bi2) == 1)){
        BigInt temp;
        temp = bi2;
        bi2 = bi1;
        bi1 = temp;
        conv = 1;
    }

    if((bi1.sign != '-' && bi2.sign == '-') || (bi1.sign == '-' && bi2.sign != '-')) // +-(a + b)
    {
        result = plus(bi1, bi2, result, max_size);
        result.sign = (bi1.sign == '-') ? ((conv == 0) ? '-' : '\0') : '\0';
    }
    else if((bi1.sign != '-' && bi2.sign != '-') || (bi1.sign == '-' && bi2.sign == '-')) //+-(a - b)
    {
        result = minus(bi1, bi2, result, max_size, conv);
        result.sign = (conv == 1 && bi1.sign == '\0') ? '-' : ((conv == 0 && bi1.sign == '-') ? '-' : '\0');
    }
    
    if(result.digits[0] == 0 && result.size == 1) result.sign = '\0';
    return result;
    }

// Function to add two BigInts and store the result in a third BigInt
BigInt add(BigInt bi1, BigInt bi2) {
    int conv = 0;
    int max_size = (bi1.size > bi2.size) ? bi1.size : bi2.size;
    //BigInt* result = (BigInt*)malloc(sizeof(BigInt)); 
    BigInt result;
    result.size = ((bi1.sign == '-' && bi2.sign == '-') || (bi1.sign != '-' && bi2.sign != '-')) ? max_size + 1 : max_size; // allocate enough space for the sum, plus a possible carry
    result.digits = (int*)malloc(result.size * sizeof(int));
    
    if(((bi1.sign != '-' && bi2.sign == '-') || (bi1.sign == '-' && bi2.sign != '-')) && (compareAbsoluteValues(bi1, bi2) == 1)){
        BigInt temp;
        temp = bi2;
        bi2 = bi1;
        bi1 = temp;
        conv = 1;
    }

    if((bi1.sign == '-' && bi2.sign == '-') || (bi1.sign != '-' && bi2.sign != '-'))
    {
        result = plus(bi1, bi2, result, max_size);
        result.sign = (bi1.sign == '-' && bi2.sign == '-') ? '-' : '\0';
    }
    else if((bi1.sign != '-' && bi2.sign == '-') || (bi1.sign == '-' && bi2.sign != '-')){
        result = minus(bi1, bi2, result, max_size, conv);
        result.sign = (bi1.sign == '-') ? ((conv == 0) ? '-' : ((conv == 1) ? '-' : '\0')) : '\0';
    }
    
    if(result.digits[0] == 0 && result.size == 1) result.sign = '\0';
    return result;
}

BigInt mul(BigInt bi1, BigInt bi2)
{
    int max_size = bi1.size + bi2.size; // maximum size of the result
    //BigInt *result = (BigInt*)malloc(sizeof(BigInt)); // allocate memory for the result
    BigInt result;
    result.size = max_size;
    result.digits = (int*)calloc(max_size, sizeof(int));
    result.sign = ((bi1.sign != '-' && bi2.sign == '-') || (bi1.sign == '-' && bi2.sign != '-')) ? '-' : '\0';

    for (int i = 0; i < bi1.size; i++) {
        for (int j = 0; j < bi2.size; ++j) {
            int digit = bi1.digits[i] * bi2.digits[j]; // multiply the two digits
            int carry = 0;
            int k = i + j;

            // add the digit to the corresponding position in the result
            while (digit || carry) {
                int sum = result.digits[k] + digit % 10 + carry;
                result.digits[k] = sum % 10;
                carry = sum / 10;
                digit /= 10;
                ++k;
            }
        }
    }

    // remove any leading zeros from the result
    while (result.size > 1 && result.digits[result.size - 1] == 0) {
        result.size--;
    }

    return result;
}

BigInt divide(BigInt bi1, BigInt bi2)
{
    BigInt result, remainder, temp0, temp1;
    result.digits =(int*)calloc(bi1.size, sizeof(int));
    remainder.digits =(int*)calloc(bi1.size, sizeof(int));
    temp0.size = 1;
    temp0.digits = (int *)calloc(temp0.size, sizeof(int));
    temp1.size = 1;
    temp1.digits = (int *)calloc(temp1.size, sizeof(int));
    temp1.digits[0] = 1;
    result.size = 0;
    remainder.size = bi1.size;
    memcpy(remainder.digits, bi1.digits, remainder.size * sizeof(int));

    char sign = ((bi1.sign != '-' && bi2.sign == '-') || (bi1.sign == '-' && bi2.sign != '-')) ? '-' : '\0';
    bi1.sign = '\0';
    bi2.sign = '\0';
    remainder.sign = '\0';

    if (bi2.size == 1 && bi2.digits[0] == 0) {
        printf("Error: division by zero\n");
        return result;
    }

    while (compareAbsoluteValues(remainder, bi2) == 0) 
    {
        remainder = sub(remainder, bi2);
        result = add(result, temp1);
    }

    while (result.size > 1 && result.digits[result.size - 1] == 0) {
        result.size--;
    }

    if(result.size == 0)
    {
        result.digits = (int *)realloc(result.digits, sizeof(int));
        result.digits[0] = 0;
        result.size = 1;
    }

    free(remainder.digits);
    free(temp0.digits);
    free(temp1.digits);

    result.sign = sign;

    return result;
}