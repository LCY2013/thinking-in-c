//
// Created by fufeng on 2022/5/28.
// 在 C 语言中，关于数据使用还有一个值得注意的问题：变量类型的隐式转换（Implicit Type Conversion）。
// C 语言作为一种弱类型语言，其一大特征就是在某些特殊情况下，变量的实际类型会发生隐式转换。
#include "stdio.h"
#include "stdlib.h"

// 在下面这个例子中，定义的两个变量 x 与 y 分别为有符号整数和无符号整数，且变量 x 的值明显小于变量 y，
// 但程序在实际运行时却会进入到 x >= y 的分支中，这就是因为发生了变量类型的隐式转换。
int main(void) {
    int x = -10;
    unsigned int y = 1;
    if (x < y) {
        printf("x is smaller than y.");
    } else {
        printf("x is bigger than y.");  // this branch is picked!
    }
    return EXIT_SUCCESS;
}
// 实际上，在上面的代码中，程序逻辑在真正进入到条件语句之前，变量 x 的类型会首先被隐式转换为 unsigned int ，即无符号整型。
// 而根据数据类型的解释规则，原先存放有 -10 补码的位模式会被解释为一个十分庞大的正整数，而这个数则远远大于 1。