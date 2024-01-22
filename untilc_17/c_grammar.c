//
// Created by fufeng on 2022/5/2.
//


#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <stdbool.h>

#define BOOL_TRUE 1  // 定义用到的宏常量与宏函数；
#define BOOL_FALSE 0
#define typename(x) _Generic((x), \
  unsigned short: "unsigned short int", \
  unsigned long: "unsigned long int", \
  default: "unknown") // C11 标准新引入的 _Generic 关键字，以用来实现基于宏的泛型；
/*
 *  编译器对 C 源代码的处理过程分为几个阶段，其中，宏是最先被处理的一个部分。
 *  在这段代码的开头处，我们通过宏指令 “#include” 引入了程序正常运行需要的一些外部依赖项，这些引入的内容会在程序编译时得到替换。
 *  随后，我们又通过 “#define” 指令定义了相应的宏常量与宏函数，而其中的宏函数 typename 则使用到了 C11 标准新引入的 _Generic 关键字，以用来实现基于宏的泛型。
 *
 * 示例中使用到的C特性：
 *  使用 const 定义只读变量；
 *  使用 typedef 定义新类型；
 *  使用 static 声明静态函数；
 *  使用各类运算符；
 *  调用标准库函数；
 *  使用 enum 定义枚举类型；
 * */

typedef enum { Host, IP } IP_ADDR_TYPE;  // 定义枚举类型 IP_ADDR_TYPE，用于表示联合中生效的字段；
typedef struct {  // 定义结构 CONN；
    size_t id;
    uint16_t port;
    bool closed;
    IP_ADDR_TYPE addr_type;
    union {
        char host_name[256];
        char ip[24];
    };
} CONN; // C99 标准的 _Bool 类型（这里的宏 bool 会展开为该类型），以及来自 C11 标准的匿名联合体；

// 接收一个指向 CONN 类型对象的常量指针，所以，通过该指针，无法在函数内部修改指针所指向对象的值。
// 而这在一定程度上保证了函数仅能够拥有足够完成其任务的最小权限。
// 添加了名为 inline 的关键字。
// 通过使用该关键字，可以“建议”编译器将该函数的内部逻辑直接替换到函数的调用位置处，以减少函数调用时产生的开销。
// 这种方式通常使用在那些函数体较小，且会被多次调用的函数上，以产生较为显著的性能提升。
inline static const char* findAddr(const CONN* pip) {  // 定义函数 findAddr，用于打印 CONN 对象的信息；
    // 动态断言，运行时
    assert(pip != NULL);  // 运行时断言，判断传入的 CONN 指针是否有效；
    return pip->addr_type == Host ? pip->host_name : pip->ip;
}

int main(int argc, char* argv[]) {  // 入口函数；
    //  C11 标准中提供的静态断言能力（编译时），来保证结构类型 CONN 的大小不会超过一定的阈值；
    static_assert(sizeof(CONN) <= 0x400, "the size of CONN object exceeds limit.");  // 静态断言，判断 CONN 对象的大小是否符合要求；
    const CONN conns[] = {  // 构造一个数组，包含三个 CONN 对象；
            [2] = { 1, 80, BOOL_TRUE, IP, { .ip = "127.0.0.1" } },
            [0] = { 2, 8080, BOOL_FALSE, IP, { .ip = "192.168.1.1" } },
            { 3, 8088, BOOL_FALSE, Host, { .host_name = "http://localhost/" } }
    };

    for (size_t i = 0; i < (sizeof(conns) / sizeof(CONN)); ++i) {  // 遍历上述 CONN 数组，并打印其中的内容；
        printf(
                "Port: %d\n"
                "Host/Addr: %s\n"
                "Internal type of `id` is: %s\n\n",
                conns[i].port,
                findAddr(&conns[i]),
                typename(conns[i].id)
        );
    }
    return EXIT_SUCCESS;
}

/*
编写完一段 C 代码，接下来的步骤就是对这段代码进行编译了。
在执行编译命令时，为了保证程序的健壮性，一般会同时附带参数 “-Wall”，让编译器明确指出程序代码中存在的所有语法使用不恰当的地方。
如果将那段用来回顾核心语法的 C 代码存放在名为 “c_grammar.c” 的文件中，那可以使用下面这行命令来编译并运行这个程序：

gcc c_grammar.c -o c_grammar -Wall && ./c_grammar

 代码预处理：编译器会首先移除源代码中的所有注释信息，并处理所有宏指令。其中包括进行宏展开、宏替换，以及条件编译等。

 编译优化：编译器会分析和优化源代码，并将其编译成对应的汇编格式代码，这部分代码中含有使用汇编指令描述的原始 C 程序逻辑。

 汇编：编译器会将这些汇编代码编译成具有一定格式，可以被操作系统使用的某种对象文件格式。

 链接：通过链接处理，编译器会将所有程序目前需要的对象文件进行整合、设置好程序中所有调用函数的正确地址，并生成对应的二进制可执行文件。

 编译结束后，我们就得到了可以直接运行的二进制文件。在不同的操作系统上，你可以通过不同的方式来运行这个程序，比如双击或通过命令行。


 */