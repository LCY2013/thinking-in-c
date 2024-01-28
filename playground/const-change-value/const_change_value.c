//
// Created by fufeng on 2024/1/28.
//
#include <stdio.h>

int main(void) {
    const int a = 5;
    int* ptr = (int *)&a;
    *ptr = 7;
    printf("a = %d\n", a);

    const int x = 5;
    const int *p = &x;

    // Attempt to modify the const-qualified variable using a non-const pointer
    *((int *)p) = 10;

    // Print the modified value
    printf("Modified value: %d\n", x);
    return 0;
}
