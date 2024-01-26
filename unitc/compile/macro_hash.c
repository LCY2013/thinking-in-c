#include <stdio.h>

//gcc -O0 -Wall -E ./macro_hash.c -o macro_hash.l

#define STRINGIFY(x) #x
#define CONCAT(x, y) x##y

int main(void) {
    printf("%s\n", STRINGIFY(Hello)); // 输出 "Hello"
    int xy = CONCAT(10, 20); // 变成 int xy = 1020;
    return 0;
}
