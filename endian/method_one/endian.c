//
// Created by fufeng on 2022/5/1.
//

#include "endian.h"
#include <stdio.h>

/*
 *  方式一： 借助联合体union的特性实现
 *  联合体类型数据所占的内存空间等于其最大的成员所占的空间，对联合体内部所有成员的存取都是相对于该联合体基地址的偏移量为 0 处开始，也就都是从该联合体所占内存的首地址位置开始。
 *
 *  说明：

    赋值 1 是数据的低字节位(0x00000001)。

    如果 1 被存储在 data所占内存 的低地址中，那data.b 的值将会是 1，就是小端模式。

    如果 1 被存储在 data所占内存 的高地址中，那data.b 的值将会是 0，就是大端模式。

 * */
__attribute__((unused)) int method_one() {
//int main() {
    union {
        int a;  // 4 bytes
        char b; // 1 bytes
    } data;

    data.a = 1; //占4 bytes，十六进制可表示为 0x 00 00 00 01

    //b因为是char型只占1Byte，a因为是int型占4Byte
    //所以，在联合体data所占内存中，b所占内存等于a所占内存的低地址部分
    if (1 == data.b) {
        //走该case说明a的低字节，被取给到了b，即a的低字节存在了联合体所占内存的(起始)低地址，符合小端模式特征
        printf("Little_Endian\n");
    } else {
        printf("Big_Endian\n");
    }
    return 0;
}


