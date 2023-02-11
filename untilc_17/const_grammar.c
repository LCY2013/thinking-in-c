//
// Created by magicLuoMacBook on 2022/5/17.
//
#include "stdio.h"

// 试着在编译指令中添加 “-pedantic -Werror” 这两个选项，然后看看结果会有什么不同？
// 实际上 Clang 在某些情况下会采用名为 “gnu-folding-constant” 的 GNU 扩展来编译 C 代码，但这并不是 C 标准中的内容。
int main(void) {
    const int vx = 10;
    const int vy = 10;
    int arr[vx] = {1, 2, 3};  // [错误1] 使用非常量表达式定义定长数组；
    switch(vy) {
        case vx: {  // [错误2] 非常量表达式应用于 case 语句；
            printf("Value matched!");
            break;
        }
    }
    //size_t
}