//
// Created by fufeng on 2022/5/17.
//

#include <stdio.h>
int main(void) {
    // 1000 1010 原码
    // 1111 0101 反码
    // 1111 0110 补码
    signed char x = -10;
    // 2^7 + 2^6 + 2^5 + 2^4 + 2^2 + 2 = 128 + 64 + 32 + 16 + 4 + 2 = 246
    unsigned char y = (unsigned char)x;
    printf("%d\n", y);  // output: 246.
    return 0;
}