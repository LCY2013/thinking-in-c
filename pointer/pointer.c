//
// Created by magicLuoMacBook on 2022/7/26.
//
#include "stdio.h"

void hello() {
    printf("Hello world!\n");
}

int main() {
    //有一个指向某个函数的指针，如果将其解引用成一个列表，然后往列表中插入一个元素，请问会发生什么？
    //（对比不同语言，看看这种操作是否允许，如果允许会发生什么）
    //把指针指向的地址强制解引用成一个列表。在 C 里这是可行的，但访问时可能会造成 segment fault。
    char buf[1024];
    void (* p)() = &hello;
    (*p)();
    int *p1 = (int *) p;
    p1[1] = 0xdeadbeef;
}