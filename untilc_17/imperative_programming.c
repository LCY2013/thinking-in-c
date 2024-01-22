//
// Created by fufeng on 2022/5/10.
//
#include <stdlib.h>
#include <stdio.h>

#define ARR_LEN 5


// 命令式编程（Imperative Programming）是这样一种编程范式
int main(void) {
    int arr[ARR_LEN] = {10,8,3,23,5};
    for (int i = 0; i < ARR_LEN; i++) {
        if (arr[i] > 7) {
            printf("%d\t", arr[i]);
        }
    }
    return EXIT_SUCCESS;
}

/*
 * 相对于命令式编程语言，其他语言一般会被归类为“声明式”编程语言。
 * 声明式编程（Declarative Programming）也是一种常见的编程范式。
 * 不同的是，这种范式更倾向于表达计算的逻辑，而非解决问题时计算机需要执行的具体步骤。
 *
 *
    构建一个容器来存放数据；
    按照条件对容器数据进行筛选，并将符合条件的数据作为结果返回。
    如果以 JavaScript 为例，对应的代码可能如下所示：
    let arr = [1, 5, 10, 9, 0]
    let result = arr.filter(n => n > 7)
 * */