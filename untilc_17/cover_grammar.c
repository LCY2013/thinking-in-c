//
// Created by magicLuoMacBook on 2022/5/17.
//

#include <stdio.h>
int main(void) {
    signed char x = -10;
    unsigned char y = (unsigned char)x;
    printf("%d\n", y);  // output: 246.
    return 0;
}