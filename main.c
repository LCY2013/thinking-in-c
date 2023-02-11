#include <stdio.h>

/*
 * 我们可以通过不同的寄存器别名来读写同一寄存器不同位置上的数据。
 * 当某个指令需要重写寄存器的低 16 位或低 8 位数据时，寄存器中其他位上的数据不会被修改。
 * 而当指令需要重写寄存器低 32 位的数据时，高 32 位的数据会被同时复位，即置零。
 *
 * 当指令作用于寄存器的不同部分时，CPU 对寄存器其他部分的影响。
 * 这里我们将值 0x100000000 放入寄存器 rax 中，在该 64 位值（long）对应的二进制编码中，其第 32 位被置位。
 * 第一句汇编指令将值 0x1 通过 movl 移动到 rax 寄存器的低 32 位；
 * 而第二句汇编指令将值 0x1 通过 movw 移动到 rax 寄存器的低 16 位。
 * 那么，通过这两种方式分别处理后的变量 num 的值是否相同呢？
 *
 * ==========  代码统计工具 cloc =================
 * brew install cloc
 * 命令：
 *  cloc dir
 *
 *fufeng@magic ~/s/k/c/kubernetes>cloc .                                                                                                master
   22870 text files.
   17831 unique files.
    5513 files ignored.

9 errors:
Line count, exceeded timeout:  ./pkg/auth/authorizer/abac/abac_test.go
Line count, exceeded timeout:  ./plugin/pkg/auth/authorizer/rbac/bootstrappolicy/policy.go
Line count, exceeded timeout:  ./staging/src/k8s.io/api/admissionregistration/v1beta1/types_swagger_doc_generated.go
Line count, exceeded timeout:  ./staging/src/k8s.io/api/extensions/v1beta1/types_swagger_doc_generated.go
Line count, exceeded timeout:  ./vendor/golang.org/x/net/idna/tables10.0.0.go
Line count, exceeded timeout:  ./vendor/golang.org/x/net/idna/tables11.0.0.go
Line count, exceeded timeout:  ./vendor/golang.org/x/net/idna/tables12.0.0.go
Line count, exceeded timeout:  ./vendor/golang.org/x/net/idna/tables13.0.0.go
Line count, exceeded timeout:  ./vendor/golang.org/x/net/idna/tables9.0.0.go

github.com/AlDanial/cloc v 1.92  T=105.71 s (168.7 files/s, 61716.5 lines/s)
--------------------------------------------------------------------------------
Language                      files          blank        comment           code
--------------------------------------------------------------------------------
Go                            14879         500628         949098        3810545
JSON                            448              3              0         890831
YAML                           1295            652           1208         133162
Markdown                        465          20294              0          71601
Bourne Shell                    334           6349          12339          31217
Protocol Buffers                115           5562          18585          11532
PO File                          12           1873          13413          11291
Assembly                         93           2555           2583           9584
PowerShell                        7            392           1017           2470
make                             60            539            895           1999
C/C++ Header                      1            399           4367            839
Bourne Again Shell               12             89             74            773
Starlark                         21             56              0            761
Lua                               1             30             26            453
sed                               4              4             32            445
Dockerfile                       50            218            721            444
Python                            7            119            159            412
SVG                               4              4              4            378
ANTLR Grammar                     1             31             17            138
C                                 5             40             68            133
XML                               5              0              0            109
TOML                              5             24             86             74
INI                               1              2              0             10
HTML                              3              0              0              3
DOS Batch                         1              2             17              2
Bazel                             1              0              0              1
CSV                               1              0              0              1
--------------------------------------------------------------------------------
SUM:                          17831         539865        1004709        4979208
--------------------------------------------------------------------------------
 */
int main() {
    register long num asm("rax") = 0x100000000;
    asm("movl $0x1, %eax");       // 4294967296 100000000
    //asm("movw $0x1, %ax");      // 4294967296 100000000
    //printf("%ld\n", num);
    printf("%lx\n", num);       // 100000000
    //grammarMain();
    return 0;
}
/*
为啥main函数里的 asm("movl $0x1, %eax"); 是将1给eax的，src 跟dst反过来了?

这里在代码中使用的是 AT&T 的写法，是一种默认被编译器广泛支持的内联汇编写法。当然，我们也可以做适当的调整来使用 Intel 写法，比如这样：

#include <stdio.h>
int main(void) {
  register long num asm("rax") = 0x100000000;
  asm(
    ".intel_syntax noprefix \n\t"
    "mov eax, 1\n\t"
    // "mov ax, 1\n\t"
    ".att_syntax"
  );
  printf("%ld\n", num);
  return 0;
}
但这种方式的问题在于，对于某些汇编器可能没有很好的兼容性。

汇编助记符 mov 是如何被翻译为二进制的 ?
每一个汇编指令都有其对应的组成结构，汇编器会根据助记符的名称进行相应的转换。具体的转换细节可以参考官方手册。比如对于 x86-64：https://www.intel.com/content/www/us/en/developer/articles/technical/intel-sdm.html

那根据机器码二进制能否推导出汇编代码呢 ?
二进制状态下的机器指令识别可以通过每个指令对应的 OpCode 字节进行切分，然后再根据相应的结构解析出整体指令的结构。


当所有指令执行完毕时，寄存器 eax 中的值是多少？
mov eax, 0x1
inc eax
sub eax, 10
xor eax, eax  // 异或
add eax, 1
mul eax

xor eax，eax 相当于把eax置零了，最后+1，然后mul相乘 默认与eax相乘，最后还是为1


arm: https://developer.arm.com/documentation/ddi0487/ga
 */
