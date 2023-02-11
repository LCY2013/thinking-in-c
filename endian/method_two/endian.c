//
// Created by magicLuoMacBook on 2022/5/1.
//

#include "endian.h"
#include <stdio.h>

/*
 *  方式二： 通过将int强制类型转换成char单字节，判断起始存储位置内容实现。
 *
 *  说明：

  赋值 1 是数据的低字节位(0x00000001)。

  如果 1 被存储在 a所占内存 的低地址中，那b的值将会是 1，就是小端模式。

  如果 1 被存储在 a所占内存 的高地址中，那b的值将会是 0，就是大端模式。

 * */
__attribute__((unused)) int method_two() {
    int a = 1;  //占4 bytes，十六进制可表示为 0x 00 00 00 01

    //b相当于取了a的低地址部分
    char *b = (char*) &a; //占1 byte

    if (1 == *b) {//走该case说明a的低字节，被取给到了b，即a的低字节对应a所占内存的低地址，符合小端模式特征
        printf("Little_Endian!\n");
    } else {
        printf("Big_Endian!\n");
    }
    return 0;
}


