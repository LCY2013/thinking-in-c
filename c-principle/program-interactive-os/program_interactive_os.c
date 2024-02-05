//
// Created by fufeng on 2024/2/5.
//

// 程序如何与操作系统交互？
//
//可以将操作系统内核暴露的“系统调用”也作为 API 的一种具体表现形式，因为调用者可以通过这些接口来使用内核提供的某种能力，但是却无需了解它们的内部实现细节。那么，系统调用究竟是什么？它与我们编写的应用程序函数有何不同？通常情况下它又是怎样实现的呢？

// 什么是系统调用？
//
//操作系统调用实际上是由操作系统内核封装好的一些可供上层应用程序使用的接口。这些接口为应用提供了可以按照一定规则访问计算机底层软件与硬件相关服务（如 IO、进程、摄像头等）的能力。其中，内核作为中间层，隔离了用户代码与硬件体系。
//
//接下来通过一个简单的例子，来看一下如何在 x86-64 平台上使用系统调用。在大多数情况下，位于内核之上的各类系统库（如 glibc、musl）会将这些系统调用按照不同类别进行封装，并提供可以直接在 C 代码中使用的函数接口。通过这种方式就可以间接地使用系统调用。当然，在这些函数内部，系统调用的具体执行通常是由汇编指令 `syscall` 完成的。
//
//比如，POSIX 标准中有一个名为 getpid 的函数，该函数用于获取当前进程的唯一标识符（ID）。如果查看 musl 中该函数针对 x86-64 平台的具体实现，会发现这样一段代码：
//
//#include <unistd.h>
//#include "syscall.h"
//pid_t getpid(void) {
//  return __syscall(SYS_getpid);
//}
//
//这段 C 代码作为“封装层（Wrapper）”，向上屏蔽了内部的 sys_getpid 系统调用。函数实现中传入的 SYS_getpid 是一个值为 39 的宏常量，这个值便为 sys_getpid 系统调用对应的唯一 ID。紧接着，通过名为 __syscall 的另一个函数，程序可以执行系统调用，并获取由内核返回的相关数据。这里，该函数直接由汇编代码定义，内容如下所示：
//
//.global __syscall
//.type __syscall,@function
//__syscall:
//  movq %rdi, %rax
//  movq %rsi, %rdi
//  movq %rdx, %rsi
//  movq %rcx, %rdx
//  movq %r8, %r10
//  movq %r9, %r8
//  movq 8(%rsp), %r9
//  syscall
//  ret
//
//在这段 AT&T 格式的汇编代码中，第 4 行的 `movq` 指令将传入该函数的系统调用 ID 存放到了 rax 寄存器中；接下来的第 5~10 行，代码将系统调用需要使用的参数也放到了相应寄存器中（这里可以看到 SysV 规范中对于普通函数和系统调用参数的不同处理方式）；最后，代码的第 11 行，通过 `syscall` 指令，系统调用得以被正确执行。
//
//可以看到，系统调用的使用十分简单。那么有没有思考过这个问题：为什么系统调用需要使用特殊的 `syscall` 指令，而非“普通”的 `call` 指令进行调用呢？那就要先从二者之间不同的代码执行环境开始说起了。

// 系统调用 vs 用户函数
//
//系统调用与一般函数（或者说“用户函数”）的最大区别在于，**系统调用执行的代码位于操作系统底层的内核环境中，而用户函数代码则位于内核之上的应用环境中**。这两种环境有时也被称为内核态与用户态。
//
//现代计算机通常采用名为“保护环（Protection Rings）”的机制来保护整个系统的数据和功能，使其免受故障和外部恶意行为的伤害。这种方式通过提供多种不同层次的资源访问级别，即“特权级别”，来限制不同代码的执行能力。
//
//比如，在 Intel x86 架构中，特权级别被分为 4 个层次，即 Ring0~Ring3。其中，Ring0 层拥有最高特权，它具有对整个系统的最大控制能力，内核代码通常运行于此。相对地，Ring3 层只有最低特权，这里是应用程序代码所处的位置。而位于两者之间的 Ring1 和 Ring2 层，则通常被操作系统选择性地作为设备驱动程序的“运行等级”。可以通过下面这张图（图片来自 [Wikipedia](<https://en.wikipedia.org/wiki/Protection_ring>)）来直观地理解特权级别的概念。
//
//![图片](.img/protection-rings.png)
//
//根据特权级别的不同，CPU 能够被允许执行的机器指令以及可使用的寄存器也有所不同。比如位于 Ring3 层的应用程序，可以使用最常见的通用目的寄存器，并通过 `mov` 指令操作其中存放的数据。而位于 Ring0 层的内核代码则可以使用除此之外的 cr0、cr1 等控制寄存器，甚至通过 `in` 与 `out` 等机器指令，直接与特定端口进行 IO 操作。但如果应用程序尝试跨级别非法访问这些被限制的资源，CPU 将抛出相应异常，阻止相关代码的执行。
//
//对于“为什么系统调用需要通过特殊的机器指令来使用”这个问题应该已经有了答案。系统调用是由内核提供的重要能力，而这些能力的具体实现代码属于内核代码的一部分。因此，为了执行这些代码，便需要一种能够在 Ring0 层将它们触发的方法，而 `syscall` 指令便能够做到这一点。
//

// 系统调用的基本实现
//
//事实上，在 x86-64 体系中，可以采用多种方式来执行一个系统调用。
//
//extern sub
//global _start
//section .text
//_start:
// and rsp,0xfffffffffffffff0
// sub rsp, 3
// mov esi, 2
// mov edi, 1
// call sub
// # use "int" to invoke a system call.
// mov ebx, eax
// mov eax, 1
// int   0x80
//
//观察上述代码的最后三行，会发现触发 exit 系统调用的方式发生了变化。`int` 指令是一个用于产生软中断的汇编指令，它在调用时会接收一个中断号作为参数。
//
//当中断发生时，执行环境会从 Ring3 切换至 Ring0，以准备执行内核代码。在这里，CPU 会首先根据 `int` 指令的参数，来从名为“中断向量表”的结构中查找下一步需要执行的中断处理程序。这里对于中断号 0x80 来说，其对应的中断处理程序便专门用于处理，由用户程序发起的系统调用请求。紧接着，这个处理程序会根据**程序通过寄存器 eax 传入的系统调用号**，来再次查找待执行的系统调用函数。最后，通过 ebx 等寄存器，系统调用函数可以获得所需参数，并完成内核某段具体代码的执行过程。
//
//在这个过程中发生了特权级别的转换，因此，为了通过隔离执行环境来保证内核安全，CPU 在进入内核态前，通常还会进行栈的切换。比如在 Linux 中，CPU 在用户态与内核态会使用不同的栈，来分别处理发生在不同特权级别下的函数调用等过程。每一个进程都对应于独立的内核栈，这个栈中会首先存放与用户态代码执行环境相关的一系列寄存器（如 esp、eip 等）的值。而当发生在内核态的相关过程（如系统调用）结束后，进程使用的栈还需要从内核栈被再次切换回用户栈，并同时恢复保存的寄存器值。
//
//可以通过下图来观察上述系统调用的整个执行过程：
//
//![](.img/system-call-process.png)
//
//以上便是通过 `int` 指令来进行系统调用的大致过程。需要注意的是，虽然我们可以在 x86-64 体系上使用这种方式，但它实际仅适用于 i386 体系。在这里，我只是以这种最经典的使用方式为例，来向你展现系统调用的一种基本实现原理。
//
//而在目前被广泛使用的 x86-64 体系中，通过 `syscall` 指令进行系统调用仍然是最高效，也最具兼容性的一种方式。`syscall` 指令的全称为“快速系统调用（Fast System Call）”，CPU 在执行该指令时不会产生软中断，因此也无需经历栈切换和查找中断向量表等过程，执行效率会有明显的提升。

//
//系统调用是由操作系统内核封装好的，一些可供上层应用程序使用的接口，这些接口可以让程序方便、安全地通过使用内核的能力，来间接地与底层软件和硬件进行交互。
//
//与用户函数不同的是，系统调用需要 CPU 执行位于内核中的代码，而现代计算机采用的“保护环”机制则将整个系统的资源访问能力划分为了多个不同的特权级别。其中，Ring0 层拥有最大执行权限，它也是内核代码的运行所在。而 Ring3 层则仅有最小权限，它是上层应用程序的默认运行层级。
//
//系统调用的经典实现方式是通过基于 `int` 指令的软中断进行的。借助软中断，CPU 可以从中断向量表中找到专门用于处理系统调用的中断处理程序，而该程序再通过由特定寄存器传入的系统调用号，来执行相应的系统调用函数。在这个过程中，操作系统通常会进行由用户栈到内核栈的转换，以及相关寄存器的存储过程。
//
//而在 x86-64 体系中，通过 `syscall` 指令进行的系统调用，由于不需要进行软中断和表查询，通常会有着更高的执行效率。

// Linux 下的 vDSO 机制是如何参与“虚拟系统调用”的执行过程的。
//
//在Linux系统中，vDSO（virtual Dynamic Shared Object）是一种机制，它允许用户空间程序直接访问一些系统调用的功能，而无需陷入内核态。vDSO是一个共享库，通常位于用户空间，它包含了一些常见的系统调用函数的快速路径，用于提高性能。
//
//vDSO机制参与“虚拟系统调用”（virtual system call）的执行过程，使得一些系统调用的执行可以在用户空间完成，而无需进入内核态。这在提高性能和减少上下文切换的开销方面是有益的。
//
//以下是虚拟系统调用的执行过程中vDSO的参与：
//
//1. 系统调用编号获取： 用户空间程序通过系统调用编号（syscall number）来指定要执行的系统调用。这个编号存储在寄存器中，通常是rax。
//
//2. 检查vDSO的可用性： 内核会检查当前系统是否支持vDSO，并且所请求的系统调用是否具备在用户空间执行的条件。如果是，就可以使用vDSO。
//
//3. vDSO快速路径执行： 如果系统调用可以由vDSO执行，内核不必进行一次完整的上下文切换到内核态执行，而是直接调用vDSO中的对应函数，从而在用户空间执行系统调用的功能。
//
//4. 系统调用返回： 执行完成后，将结果返回给用户空间程序。如果vDSO无法执行该系统调用，程序将陷入内核态，执行相应的内核处理过程，然后返回结果。
//
//vDSO通常包含一些与时间相关的系统调用，如gettimeofday，以及其他一些频繁使用的系统调用，如getpid等。由于这些系统调用的功能在vDSO中已经被实现，因此可以通过在用户空间直接调用这些函数来提高性能。
//
//需要注意的是，vDSO并非所有系统调用都可以替代，只有一些经常使用的系统调用才有可能在用户空间执行。虚拟系统调用的使用也受到系统和架构的限制，因此在实际编程中，应该检查系统的支持情况。