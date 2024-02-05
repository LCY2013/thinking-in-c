//
// Created by fufeng on 2024/2/5.
//
#include <stdio.h>
void test() {
    printf("test");
    return;
}
void stackret(long* l) {
    *l-- = (long)test;
    *l-- = (long)test;
    *l-- = (long)test;
    *l-- = (long)test;
    *l-- = (long)test;
    return;
}
int main(void) {
    int* p;
    long x = 0;
    stackret(&x);
    return 0;
}