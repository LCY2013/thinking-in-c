.global setjmp
.intel_syntax noprefix
setjmp:
  mov  QWORD PTR [rdi], rbx
  mov  QWORD PTR [rdi+0x8], rbp
  mov  QWORD PTR [rdi+0x10], r12
  mov  QWORD PTR [rdi+0x18], r13
  mov  QWORD PTR [rdi+0x20], r14
  mov  QWORD PTR [rdi+0x28], r15
  lea  rdx, [rsp+0x8]
  mov  QWORD PTR [rdi+0x30], rdx
  mov  rdx, QWORD PTR [rsp]
  mov  QWORD PTR [rdi+0x38], rdx
  xor  eax, eax
  ret