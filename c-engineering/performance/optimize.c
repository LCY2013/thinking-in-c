//
// Created by fufeng on 2024/2/1.
//

// 极致优化：如何实现高性能的 C 程序？

//四个优化 C 代码的技巧分别是利用高速缓存、利用代码内联、利用 `restrict` 关键字，以及消除不必要内存引用。

// 如何衡量程序的运行性能？
//
//首先需要知道如何衡量一个应用程序的运行性能。
//
//通常可以采用最简单和直观的两个宽泛指标，即内存使用率和运行时间。对于具有相同功能的两个程序，在完成相同任务时，哪一个的内存使用率更低，且运行时间更短，则程序的整体性能相对更好。
//
//可以将这两个指标再进一步细分。比如程序的内存使用率，可以细分为程序在单位时间内的主内存使用率、L1-L3 高速缓存使用率、程序堆栈内存使用率等指标。而运行时间也可以细分为程序总体运行时间（墙上时钟时间）、用户与操作系统时间（CPU 时间），以及空闲时间与 IO 等待时间等。
//
//至于这些指标的制定和使用方式，属于性能监控的一个话题。但需要知道的是，无论程序多么复杂，运行时间和内存消耗这两个指标都是可用于观察程序性能情况的基本指标。
//
//主要讨论可用于优化程序性能的具体编码方式。而采用这些编码方式能够保证代码在所有运行环境中，都拥有一个相对稳定且高效的执行情况。但也需要注意，程序的实际运行可能会受到操作系统、编译器、网络环境等多方面的影响，因此，即使采用这些编码方式，也并不一定保证程序可以在所有执行环境上都能够获得较大的性能提升。
//
//下面会以程序对应的 x86-64 汇编代码为例，来深入介绍各种优化技巧背后的逻辑。不过，这些基于 C 代码的性能优化策略都是相通的，只要掌握了它们的原理，在不同的平台体系上，都可以做到举一反三，运用自如。下面就来看具体的编码技巧吧。

// 技巧一：利用高速缓存
//
//在选购电脑时，经常会遇到 L1、L2、L3 高速缓存这几个指标，由此可见这些概念在衡量计算机整体质量中的重要性。那么，它们究竟代表什么呢？
//
//都清楚缓存的重要性，通过将常用数据保存在缓存中，当硬件或软件需要再次访问这些数据时，便能以相对更快的速度进行读取和使用。而在现代计算机体系中，L1、L2、L3（某些体系可能有 L4 缓存，但不普遍）一般分别对应于 CPU 芯片上的三个不同级别的高速缓存，这些缓存的数据读写速度依次降低，但可缓存数据的容量却依次升高。而这些位于 CPU 芯片上的缓存，则主要用于临时存放 CPU 在最近一段时间内经常使用的数据和指令。
//
//L1 到 L3 缓存直接由 CPU 硬件进行管理，当 CPU 想要读取某个数据时，它会按照一定规则优先从 L1 缓存中查找，如果没有找到，则再查找 L2 缓存，以此类推。而当在所有高速缓存中都没有找到目标数据时，CPU 便会直接从内存中读取。与直接从 L1 中读取相比，这个过程花费的时间会多出上百倍，所以很清楚地看到优先将数据存放在高速缓存中的重要性。
//
//高速缓存之所以能够提升性能，一个重要的前提便在于“局部性（locality）原理”。该原理通常被分为两个方面，即“时间局部性”和“空间局部性”，它们的内容分别如下所示。
//
//- 时间局部性：被引用过一次的内存位置可能在不远的将来会再被多次引用；
//- 空间局部性：如果一个内存位置被引用了一次，那么程序很可能在不久的将来引用其附近的另一个内存位置。
//
//局部性本身是处理器访问内存行为的一种趋势，因此，如果一个程序在设计时能够很好地满足局部性原理，其运行便可能会有更高的性能表现。接下来看看，如何让程序设计尽量满足这些原则。
//
//在这里，给出了一段不满足局部性原理的代码，可以先思考下这段代码有什么问题，有没有可以进一步提升性能的空间。
#define M 128
#define N 64
int sum(int a[M][N]) {
    int i, j, sum = 0;
    for (j = 0; j < N; ++j) {
        for (i = 0; i < M; ++i) {
            sum += a[i][j];
        }
    }
    return sum;
}
//可以看到的是，这段代码每次的内存访问过程（即访问数组 a 中的元素）都没有尽可能地集中在一段固定的内存区域上。相反，访存的过程发生在多个不同位置，且各个位置之间的跨度很大。这一点从代码中 sum 变量按照“列优先”顺序（先循环变量 i，后循环变量 j）访问数组元素的方式就可以得出。
//
//C 语言中二维数组的元素是按照“行优先”的方式存储的，也就是说，在上面的数组 a 中，连续不同的 j 将引用数组中连续的内存位置。CPU 在缓存数据时，会按照局部性原则，缓存第一次访问内存时其附近位置的一段连续数据。而列优先的访问方式则使得数组中数据的引用位置变得不连续，这在一定情况下可能会导致上一次已被放入高速缓存中的数据，在下一次数据访问时没有被命中。而由此带来的直接对内存的频繁访问，则会导致程序整体性能的降低。
//
//因此结论是，上述代码的设计并没有很好地满足空间局部性，其运行效率并没有被最大化。可以通过下图来形象地理解这个问题：
//
//![](.img/optimize-space-local.png)
//
//可以看到，当代码的内层循环第一次访问数组 a 时，假设 CPU 会将临近的 4 个元素放入到它的高速缓存中。而由于代码采用了“行优先”访问，因此当下一次访问数组元素时，位于高速缓存中的数据便不会被命中。以此类推，后续的每次访问都会产生这样的问题。
//
//因此，为了能更好地利用 CPU 高速缓存，可以参考这些原则来编写代码：
//
//- 尽量确保定义的局部变量能够被多次引用；
//- 在循环结构中，以较短的步长访问数据；
//- 对于数组结构，使用行优先遍历；
//- 循环体越小，循环迭代次数越多，则局部性越好。
//
//自定义数据对齐，通过合理使用 `alignas` 关键字，也可以优化某些特定场景下的代码，来让它在最大程度上利用高速缓存。来看下面这个例子：
struct data {
     char x;
     alignas(64) char y[118];
};
//在这段代码中定义了一个名为 data 的结构。它包含两个字段，一个为字符类型的 x，另一个为包含有 118 个元素的字符数组 y。而通过指定 `alignas(64)` 标识符，限制字段 y 在内存中的起始地址需要在 64 字节上对齐。
//
//实际上，在计算机内部，高速缓存是以“缓存行（Cache Line）”的形式被组织的。也就是说，一大块连续的高速缓存会被分为多个组，每个组中有多个行，而每个行则具有固定大小。当发生缓存不命中时，CPU 会将数据从低层次缓存中以固定的“块大小（通常为缓存行大小）”为单位，拷贝到高层次缓存中。而为了减少 CPU 需要进行的内存拷贝次数，希望连续的数据能被组织在尽可能少的缓存行中。
//
//另一方面，内存与高速缓存之间的映射关系一般与数据在内存中的具体地址有关。比如，对于采用“直接映射”方式的缓存来说，假设缓存行大小为 64 字节，若某段数据起始于内存中对齐到 64 字节的地址，而当它被拷贝到高速缓存中时，便会从缓存行的开头处开始放置数据，这在最大程度上减少了这段连续数据需要占用的缓存行个数（当从缓存行中间开始存放数据时，字段 y 可能需要占用三个缓存行）。上面那段代码便是如此。

// 技巧二：代码内联
//
//第二种常用于性能优化的方式是代码内联（Inlining）。这种方式很好理解，下面直接来看个例子，由此理解内联的概念和它对程序运行的影响。
//
//C99 标准引入了一个名为 `inline` 的关键字，通过该关键字，可以建议编译器，将某个方法的实现内联到它的实际调用处。来看下面这个简短的例子：
#include <stdio.h>
static inline int foo() {
    return 10;
}
void InlineFunc(void) {
    int v = foo();
    printf("Output is: %d\n", v);
}
//在这段代码中使用 `inline` 关键字标注了方法 foo，并在 main 函数内将 foo 方法的调用返回结果赋值给了变量 v。为了能够看清 `inline` 关键字对程序实际运行的影响，还需要查看上述 C 代码对应的汇编代码，具体如下所示：
//.LC0:
//        .string "Output is: %d\n"
//InlineFunc:
//        sub     rsp, 8
//        mov     esi, 10
//        mov     edi, OFFSET FLAT:.LC0
//        mov     eax, 0
//        call    printf
//        add     rsp, 8
//        ret

//可以看到，在这段汇编代码中，实际上只有 InlineFunc 函数的机器指令实现，而 foo 函数的具体定义则已经被替换到了它在 InlineFunc 函数中的实际调用位置处（对应 `mov esi, 10` 这一行） 。
//
//通过这种方式，程序不再需要使用 `call` 指令来调用 foo 函数。这样做的好处在于，可以省去 `call` 指令在执行时需要进行的函数栈帧创建和销毁过程，以节省部分 CPU 时钟周期。而通过这种方式得到的性能提升，通常在函数被多次调用的场景中更加显而易见。
//
//`inline` 固然好用，但也要注意这一点：函数本身作为 C 语言中对代码封装和复用的主体，不恰当的内联也会导致程序可执行二进制文件的增大，以及操作系统加载程序时的效率变低。一般情况下，内联仅适用于那些本身实现较为短小，且可能会被多次调用的函数。同时，`inline` 关键字也仅是程序员对编译器提出的一个建议，具体是否会被采纳，还要看具体编译器的实现。而对于大部分常用编译器来说，在高优化等级的情况下，它们也会默认采用内联来对代码进行优化。
//
//当然，除此之外，也可以选择通过宏来进行预处理时的代码内联。采用这种方式的话需要将 C 代码封装成对应的宏，并在需要内联的地方展开。

// 技巧三：`restrict` 关键字
//
//C99 标准新增了一个名为 `restrict` 的关键字，可以优化代码的执行。该关键字只能用于指针类型，用以表明该指针是访问对应数据的唯一方式。
//
//在计算机领域，有一个名为 aliasing 的概念。这个概念是说内存中的某一个位置，可以通过程序中多于一个的变量来访问或修改其包含的数据。而这可能会导致一个潜在的问题：即当通过其中的某个变量修改数据时，便会导致所有与其他变量相关的数据访问发生改变。
//
//因此，aliasing 使得编译器难以对程序进行过多的优化。而在 C 语言中，`restrict` 关键字便可以解决这个问题。如果学习过 Rust，这也是其所有权机制的核心内容。下面来看一个例子。
#include <stdio.h>
void fooRestrict(int* x, int* y, int* restrict z) {
    *x += *z;
    *y += *z;
}
void Restrict(void) {
    int x = 10, y = 20, z = 30;
    foo(&x, &y, &z);
    printf("%d %d %d", x, y, z);
}
//在这段代码中，函数 foo 共接收三个整型指针参数，它的功能是将第三个指针指向变量的值，累加到前两个指针指向的变量上。其中，第三个参数 `z` 被标记为了 `restrict` ，这表明向编译器做出了这样一个承诺：即在函数体 foo 内部，只会使用变量 `z` 来引用传入函数第三个指针参数对应的内存位置，而不会发生 aliasing。这样做使得编译器可以对函数的机器码生成做进一步优化。
//
//来看下上面这段 C 代码对应的汇编代码：
//fooRestrict:
//        mov     eax, DWORD PTR [rdx]
//        add     DWORD PTR [rdi], eax
//        add     DWORD PTR [rsi], eax
//        ret
//.LC0:
//        .string "%d %d %d"
//Restrict:
//        sub     rsp, 24
//        mov     DWORD PTR [rsp+12], 10
//        mov     DWORD PTR [rsp+8], 20
//        mov     DWORD PTR [rsp+4], 30
//        lea     rdx, [rsp+4]
//        lea     rsi, [rsp+8]
//        lea     rdi, [rsp+12]
//        mov     eax, 0
//        call    foo
//        mov     ecx, DWORD PTR [rsp+4]
//        mov     edx, DWORD PTR [rsp+8]
//        mov     esi, DWORD PTR [rsp+12]
//        mov     edi, OFFSET FLAT:.LC0
//        mov     eax, 0
//        call    printf
//        add     rsp, 24
//        ret
//
//可以发现，在使用 `restrict` 关键字标注了 fooRestrict 函数的第三个参数后，在为指针 `y` 进行值累加前，编译器不会再重复性地从内存中读取指针 `z` 对应的值。而这对程序的执行来说，无疑是一种性能上的优化。
//
//另外需要注意的是，若一个指针已被标记为 `restrict` ，但在实际使用时却发生了 aliasing，此时的行为是未定义的。

// 技巧四：消除不必要的内存引用
//
//在某些情况下，可能只需要对程序的结构稍作调整，便能在很大程度上提升程序的运行性能。可以先看看下面这段代码，思考下优化方式：
#define LEN 1024
int data[LEN] = { ... };
int fooMemRef(int* dest) {
    *dest = 1;
    for (int i = 0; i < LEN; i++) {
        *dest = *dest * data[i];
    }
}
//在这段代码中，函数 fooMemRef 主要用来计算数组 data 中所有元素的乘积，并将计算结果值拷贝给指针 dest 所指向的整型变量。函数的逻辑很简单，但当仔细观察函数 foo 内部循环逻辑的实现时，便会发现问题所在。
//
//在这个循环中，为了保存乘积在每一次循环时产生的累积值，函数直接将这个值更新到了指针 dest 指向的变量中。并且，在每次循环开始时，程序还需要再从该变量中将临时值读取出来。知道，从内存中读取数据的速度是慢于寄存器的。因此，这里可以快速想到一个优化方案。优化后的代码如下所示：
#define LEN 3
int dataOptimize[LEN] = { 1,2,4 };
int fooOptimize(int* dest) {
    register int acc = 1;
    for (int i = 0; i < LEN; i++) {
        acc = acc * data[i];
    }
    *dest = acc;
}
//在上面的代码中一共做了两件事情：
//
//1. 将循环中用于存放临时累积值的 “*dest” 替换为一个整型局部变量 “acc”；
//2. 在定义时为该变量添加额外的 `register` 关键字，以建议编译器将该值存放在寄存器中，而非栈内存中。
//
//通过消除不必要的内存引用，就能够减少程序访问内存的次数，进而提升一定的性能。

//- 高速缓存利用了 CPU 的局部性，使得满足局部性的程序可以更加高效地访问数据；
//- 代码内联通过直接使用函数定义替换函数调用的方式，减少了程序调用 `call` 指令带来的额外开销；
//- `restrict` 关键字通过限制指针的使用，避免 aliasing，进而给予了编译器更多的优化空间；
//- 消除不必要的内存引用，则是通过减少程序与内存的交互过程，来进一步提升程序的运行效率。

// 论其他几种常见的 C 代码和程序优化技巧，它们分别是利用循环展开、使用条件传送指令、尾递归调用优化，以及为编译器指定更高的编译优化等级。

//  技巧五：循环展开（Loop Unrolling）
//
//为了更好地理解“循环展开”这个优化技巧背后的原理，先从宏观角度看看 CPU 是如何运作的。
//
//早期的 CPU 在执行指令时，是以串行的方式进行的，也就是说，一个指令的执行开始，需要等待前一个指令的执行完全结束。这种方式在实现上很简单，但存在的问题也十分明显：由于指令的执行是一个涉及多个功能单元的复杂过程，而在某一时刻，CPU 也只能够对指令进行针对当前所在阶段的特定处理。
//
//那么，将 CPU 处理指令的流程划分为不同阶段，并让它对多条指令同时进行多种不同处理，这样是否可以进一步提升 CPU 的吞吐量呢？事实正是如此。
//
//现代 CPU 为了进一步提升指令的执行效率，通常会将单一的机器指令再进行拆分，以达到指令级并行的目的。比如，对于一个基本的五级 RISC 流水线来说，CPU 会将指令的执行细分为指令提取（IF）、指令译码（ID）、指令执行（EX）、内存访问（MEM），以及寄存器写回（WB）共五个步骤。
//
//在这种情况下，当第一条机器指令经过了指令提取阶段的处理后，即使该条指令还没有被完全执行完毕，CPU 也可以立即开始处理下一条机器指令。因此，从宏观上来看，机器指令的执行由串行变为了并行，程序的执行效率得到了提升。
//
//其中，指令提取是指从内存中读取出机器指令字节的过程。CPU 根据得到的指令字节，在译码阶段，从相应的寄存器中获得指令执行所需要的参数。而在执行阶段，ALU 可以选择执行指令明确的操作，或者是计算相关内存引用的有效地址等操作。随后，在访存阶段，根据指令要求，CPU 可以将数据写回内存，或从内存中读出所需数据。类似地，在写回阶段，CPU 可以将指令执行得到的结果存入寄存器。
//
//而当五个阶段全部执行完毕后，CPU 会更新指令指针（PC），将其指向下一个需要执行的指令。可以通过下图来直观地理解这个过程：
//
//![](.img/cpu-exec.png)
//
//那么，如何将 CPU 的吞吐量最大化呢？就是让 CPU 在执行程序指令时，能够以满流水线的方式进行。
//
//但现实情况并非总是这样理想。这里要介绍的代码优化技巧“循环展开”便与此有关。先来看一段代码：
#define LEN 4096
int dataLoopUnrolling[LEN] = { ... };
int fooLoopUnrolling(void) {
    int acc = 1;
    for (int i = 0; i < LEN; ++i) {
        acc = acc * dataLoopUnrolling[i];
    }
    return acc;
}
//在这段代码中定义了一个名为 dataLoopUnrolling 的全局整型数组，并在其中存放了若干个值。而函数 fooLoopUnrolling 则主要用来计算该数组中所有数字的乘积之和。
//
//此时，如果在 main 函数中调用函数 fooLoopUnrolling，CPU 在实际执行它的代码时，`for` 循环的每一轮都会产生两个数据相关：循环控制变量 i 的下一个值依赖于本次循环变量 i 在经过自增运算后得到的结果值。同样地，计数变量 acc 的下一个值也依赖于该变量在当前循环中经过乘积计算后的结果值。
//
//而这两个数据相关会导致 CPU 无法提前计算下一轮循环中各个参与变量的值。而只有在寄存器写回，或内存访问阶段执行完毕，也就是变量 acc 和 i 的值被最终更新后，CPU 才会继续执行下一轮循环。
//
//那么，应该如何优化这个过程呢？直接来看优化后的代码：
#define LEN 4096
int dataLoopUnrollingCurrent[LEN] = { ... };
int fooLoopUnrollingCurrent(void) {
    int limit = LEN - 1;
    int i;
    int acc0 = 1;
    int acc1 = 1;
    for (i = 0; i < limit; i += 2) {  // 2x2 loop unrolling.
        acc0 = acc0 * dataLoopUnrollingCurrent[i];
        acc1 = acc1 * dataLoopUnrollingCurrent[i + 1];
    }
    for (; i < LEN; ++i) {  // Finish any remaining elements.
        acc0 = acc0 * dataLoopUnrollingCurrent[i];
    }
    return acc0 * acc1;
}
//可以直观地看到，参与到程序执行的局部变量变多了。而这里的主要改动是，为函数 foo 中的循环结构应用了 2x2 循环展开。
//
//循环展开这种代码优化技术，主要通过增加循环结构每次迭代时计算的元素个数，来减少循环次数，同时优化 CPU 的指令集并行与流水线调度。而所谓的 2x2 ，是指在优化后的代码中，循环的步长变为了 2，且循环累积值被分别存放在两个独立变量 acc0 与 acc1 中。
//
//循环展开带来的最显著优化，就是减少了循环的迭代次数。使用多个独立变量存储累积值，各个累积值之间就不会存在数据相关，而这就增大了 CPU 多个执行单元可以并行执行这些指令的机会，从而在一定程度上提升了程序的执行效率。
//
//需要注意的是，循环展开一方面可以带来性能上的提升，另一方面它也会导致程序代码量的增加，以及代码可读性的降低。并且，编译器在高优化等级下，通常也会对代码采用隐式的循环展开优化。因此，在大多数情况下并不需要手动地改变代码形式来为它应用循环展开，除非是在那些确定编译器没有进行优化，并且手动循环展开可以带来显著性能提升的情况下。

// 技巧六：优先使用条件传送指令
//
//通常来说，CPU 指令集中存在着一类指令，它们可以根据 CPU 标志位的不同状态，有条件地传送数据到某个特定位置，这类指令被称为“条件传送指令”。举个例子，指令 `cmove` 接收两个参数 S 和 R，当 CPU 标志寄存器中的 ZF 置位时，该指令会将 S 中的源值复制到 R 中。
//
//与条件传送指令类似的还有另外一类指令，它们被称为“条件分支指令”。顾名思义，这类指令在执行时，会根据 CPU 标志位的不同状态，选择执行程序不同部分的代码。比如指令 `jz` ，该指令接收一个参数 L，当 CPU 标志寄存器中的 ZF 置位时，该指令会将下一条待执行指令修改为 L 所在内存位置上的指令。
//
//对于 C 代码中的某些逻辑，使用条件传送指令与条件分支指令都能够正确完成任务。但在程序的执行效率上，这两种方式却可能带来极大的差别。而这主要是由于条件分支指令可能会受到 CPU 分支预测错误带来的惩罚。
//
//现代 CPU 一般都会采用投机执行，其中的一个场景是：处理器会从它预测的，分支可能会发生跳转的地方取出指令，并提前对这些指令进行译码等操作。处理器甚至会在还未确认预测是否正确之前，就提前执行这些指令。之后，如果 CPU 发现自己预测的跳转位置发生错误，就会将状态重置为发生跳转前分支所处的状态，并取出正确方向上的指令，开始重新处理。
//
//由此，上述两种指令在 CPU 的内部执行上便产生了不同。由于条件分支指令会导致 CPU 在指令实际执行前作出选择，而当 CPU 预测错误时，状态的重置及新分支的重新处理过程会浪费较多的 CPU 周期，进而使程序的运行效率下降。相对地，条件传送指令不会修改处理器的 PC 寄存器，因此它不会导致 CPU 需要进行分支预测，也就不会产生这部分损失。
//
//至于 CPU 是如何进行分支预测的，需要查看对应资料，但需要知道的是，在发生类似问题时，可以进一步观察程序，并尝试使用条件传送指令优化这些逻辑。为了方便理解，来看个例子。可以看看下面这段代码中函数 fooConditionalTransferOrder 的实现细节：
#define LEN 1024
//条件传送指令
void fooConditionalTransferOrder(int* x, int* y) {
    int i;
    for (i = 0; i < LEN; i++) {
        if (x[i] > y[i]) {
            int t = x[i];
            x[i] = y[i];
            y[i] = t;
        }
    }
}
//函数 fooConditionalTransferOrder 接收两个整型数组 x 与 y，并依次比较这两个数组中位于相同索引位置上的元素，最后将较大者存放到数组 y 的对应位置上。可以看到，在遍历数组的过程中，在循环结构内使用了 `if` 语句，来判断数组 x 中的元素值是否大于数组 y 对应位置上的元素。而在代码实际编译时，`if` 语句通常会由对应的条件分支指令来实现。因此，在循环结构的“加持”下，由 CPU 分支预测错误引发的惩罚，在经过每一轮迭代的累积后，都可能会变得更加可观、更加明显。
//
//下面就使用条件传送指令优化这段代码。条件传送指令一般会用于实现 C 语法中的三元运算符 `?:`，因此对上述代码的优化过程也就显而易见：
#include <stdio.h>
#define LEN 16
void fooConditionalTransferOrderCurrent(int* x, int* y) {
    int i;
    for (i = 0; i < LEN; i++) {
        int min = x[i] < y[i] ? x[i] : y[i];
        int max = x[i] < y[i] ? y[i] : x[i];
        x[i] = min;
        y[i] = max;
    }
}
//可以看到，这里没有使用 `if` 语句来判断，是否应该调整两个数组对应位置上的数字值，而是直接使用三元运算符，来将每一次迭代时的最大值与最小值结果计算出来，并拷贝到数组中的相应位置上。
//
//通过这种方式，虽然解决了 CPU 分支预测失败带来的惩罚，但与此同时，每一次循环中也会多了几次比较与赋值操作。这样的一来一回真的可以提升性能吗？不要小看 CPU 指令并行处理能力的高效性，但也不要小看 CPU 分支预测带来的性能损耗。

// 技巧七：使用更高的编译优化等级
//
//除了可以通过调整代码写法来优化程序运行外，还可以为编译器指定更高优化等级的选项，来让编译器自动进行更多程序执行细节上的优化。
//
//以 GCC 为例，它提供了 -O0、-O1、-O2、-O3、-Os、-Ofast 等优化选项。把它们各自的大致优化内容整理成了一张表格，你可以参考：<br>
//
//![](.img/gcc-optimize.png)

//## 技巧八：尾递归调用优化（Tail-Call Optimization）
//
//尾递归调用优化也是一个重要的代码优化技巧。关于它的原理和代码编写方式。
//
//总的来看，尾递归调用优化通过将函数的递归调用过程优化为循环结构，减少了程序执行时对 `call` 指令的调用次数，进而减少了栈帧的创建与销毁过程，提升了程序的执行性能。并且需要注意尾递归调用优化的效果在那些函数体本身较小，且递归调用次数较多的函数上体现得会更加明显。

//- 循环展开让我们可以进一步利用 CPU 的指令级并行能力，让循环体执行得更快；
//- 优先使用条件传送指令，让我们可以在一些特定的场景中，防止使用条件分支指令带来的 CPU 周期浪费；
//- 使用更高的编译优化等级，让我们可以借编译器之手，利用更多“黑科技”进一步优化我们的代码；
//- 尾递归调用优化让我们可以用循环代替递归，减少函数调用时的栈帧创建与销毁过程，让递归进行得更快。