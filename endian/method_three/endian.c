//
// Created by fufeng on 2022/5/1.
//

#include "endian.h"
#include <stdio.h>

/*
 *  方式三： 用MSB和LSB讲大端和小端的描述，我们一定需要注意，大端和小端描述的是字节之间的关系，而MSB、LSB描述的是Bit位之间的关系。字节是存储空间的基本计量单位，所以通过高位字节和低位字节来理解大小端存储是最为直接的。

  MSB: Most Significant Bit ------- 最高有效位(指二进制中最高值的比特)

  LSB: Least Significant Bit ------- 最低有效位(指二进制中最高值的比特)

  当然，也可以通过MSB/LSB实现大端小端的判断和检测，举例代码如下：
 * */
__attribute__((unused)) int method_three() {
    union {
        struct {
            char a:1; //定义位域为 1 bit
        } s;
        char b;
    } data;

    data.b = 8;//8(Decimal) == 1000(Binary)，MSB is 1，LSB is 0

    //在联合体data所占内存中，data.s.a所占内存bit等于data.b所占内存低地址部分的bit
    if (1 == data.s.a) {//走该case说明data.b的MSB是被存储在union所占内存的低地址中，符合大端序的特征
        printf("Big_Endian\n");
    } else {
        printf("Little_Endian\n");
    }
    return 0;
}


