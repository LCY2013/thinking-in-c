.global longjmp
.intel_syntax noprefix
longjmp:
  xor  eax, eax
  cmp  esi, 0x1
  adc  eax, esi
  mov  rbx, QWORD PTR [rdi]
  mov  rbp, QWORD PTR [rdi+0x8]
  mov  r12, QWORD PTR [rdi+0x10]
  mov  r13, QWORD PTR [rdi+0x18]
  mov  r14, QWORD PTR [rdi+0x20]
  mov  r15, QWORD PTR [rdi+0x28]
  mov  rsp, QWORD PTR [rdi+0x30]
  jmp  QWORD PTR [rdi+0x38]