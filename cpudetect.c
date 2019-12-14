extern int cpudetect(void);

/*
 * Detects x86 mode and CPU type, returns (in EAX, or AX in 16-bit mode):
 *
 * 0: 16-bit mode (CPU can be any of: 8088, 8086, 80186 or 80286, >=i386)
 * 2: 32-bit mode, i486 without CPUID
 * 3: 32-bit mode, i386 (without CPUID)
 * 4: 32-bit mode, i486 with CPUID
 * 5: 32-bit mode, i586 (with CPUID, P5 microarchitecture)
 * 6: 32-bit mode, i686 (with CPUID, P6 microarchitecture, >= Pentium Pro)
 * 7: 64-bit mode (>=amd64, at least P6 microarchitecture, from AMD Athlon K7)
 *
 * It uses <=16 bytes of stack, it saves and restores all registers (except for
 * EAX) and flags. It's position-independent code.
 *
 * The source code below works and produces identical code when compiled with
 * `gcc -m32' or `gcc -m64'. It also works with `tcc -m32' and `tcc -m64'.
 */
#ifndef __i386__
#ifndef __x86_64__
#ifndef __ia16__
#error cpudetects needs i386, amd64 (x86_64) or ia16 architecture
#endif
#endif
#endif
int cpudetect(void) {
#ifdef __x86_64__  /* Shortcut, also works without it. */
  return 7;
#else
#ifdef __ia16__  /* Shortcut, also works without it. */
  return 0;
#else
  int res;
  __asm__ __volatile__ (
  ".byte 0x9c\n"  /* 16-bit=(pushf); 32-bit=(pushfd); 64-bit=(pushfq) */
  ".byte 0xb8\n"  /* mov eax, imm32 */
  ".long 0xc8fe0002\n"  /* Based on: https://codegolf.stackexchange.com/a/139717 */
  ".byte 0x48, 0x90\n"  /* 32-bit=(dec eax + nop); 64-bit=(xchg rax, rax) */
  ".short 0xe083\n"  /* and eax, imm8 */
  ".byte 3\n"
  ".short 0xc085\n"  /* test eax, eax */
  "jz cpudetect__pr\n"  /* 16-bit mode: return 0 in AX. */
  ".short 0xf883\n"  /* cmp eax, imm8 */
  ".byte 2\n"
  "mov $7, %%al\n"
  "je cpudetect__pr\n"  /* 64-bit mode: return 2 in EAX. */
  /* TODO(pts): Do CPUID. The first 64-bit CPU was AMD Athlon (== K7), which */
  /* has CPUID family 6 (P6 microarchitecture) already. */
  /* 32-bit mode. */
  ".byte 0x51\n"  /* push ecx  (doesn't exist in 64-bit mode) */
  ".byte 0x9c\n"  /* pushfd  (doesn't exist in 64-bit mode) */
  ".byte 0x58\n"  /* pop eax  (doesn't exist in 64-bit mode) */
  ".short 0xc189\n"  /* mov ecx, eax */
  ".byte 0x35\n"  /* xor eax, imm32 */
  ".long 0x200000\n"  /* Flip ID (identification) bit in EFLAGS. */
  ".byte 0x50\n"  /* push eax  (doesn't exist in 64-bit mode) */
  ".byte 0x9d\n"  /* popfd  (doesn't exist in 64-bit mode) */
  ".byte 0x9c\n"  /* pushfd  (doesn't exist in 64-bit mode) */
  ".byte 0x58\n"  /* pop eax  (doesn't exist in 64-bit mode) */
  ".short 0xc831\n"  /* xor eax, ecx */
  "jnz cpudetect__wc45\n"  /* Can toggle ID bit, so it's i486 or >=i586 with CPUID. */
  ".short 0xc889\n"  /* mov eax, ecx */
  /* TODO(pts): Is it safe to set AC? What if a signal is delivered? cli */
  /* to temporarily disable interrupts doesn't work in protected mode. Does */
  /* the signal handler clear the AC flag first? Can we test without AC? */
  /* TODO(pts): Is bit 18 set in cr0? */
  /* https://news.ycombinator.com/item?id=18468845 */
  ".byte 0x35\n"  /* xor eax, imm32 */
  ".long 0x40000\n"  /* Flip AC (alignment check) bit in EFLAGS. */
  ".byte 0x50\n"  /* push eax  (doesn't exist in 64-bit mode) */
  ".byte 0x9d\n"  /* popfd  (doesn't exist in 64-bit mode) */
  ".byte 0x9c\n"  /* pushfd  (doesn't exist in 64-bit mode) */
  ".byte 0x58\n"  /* pop eax  (doesn't exist in 64-bit mode) */
  /* If can't toggle AC bit, it's i386, will return 3 (after dec). */
  ".short 0xc831\n"  /* xor eax, ecx */
  ".byte 0xb8\n"  /* mov eax, imm32 */
  ".long 3\n"
  "jz cpudetect__4\n"
  /* If can toggle AC bit, it's i486 without CPUID, return 2 in EAX. */
  ".byte 0x48\n"  /* dec eax  (has a different opcode in 64-bit mode) */
  "cpudetect__4:"
  ".byte 0x51\n"  /* push ecx  (doesn't exist in 64-bit mode) */
  ".byte 0x9d\n"  /* popfd  (doesn't exist in 64-bit mode) */
  ".byte 0x59\n"  /* pop ecx  (doesn't exist in 64-bit mode) */
  "jmp cpudetect__pr\n"
  "cpudetect__wc45:"  /* i486 or >=i586 with CPUID. */
  ".byte 0x51\n"  /* push ecx  (doesn't exist in 64-bit mode) */
  ".byte 0x9d\n"  /* popfd  (doesn't exist in 64-bit mode) */
  ".short 0xc031\n"  /* xor eax, eax */
  ".byte 0x40\n"  /* inc eax  (has a different opcode in 64-bit mode) */
  ".byte 0x53\n"  /* push ebx  (doesn't exist in 64-bit mode) */
  ".byte 0x52\n"  /* push edx  (doesn't exist in 64-bit mode) */
  "cpuid\n"  /* EAX == 1, get processor info and features bits. */
  /* Restore registers EDX, ECX and EBX after CPUID. */
  ".byte 0x5a\n"  /* push edx  (doesn't exist in 64-bit mode) */
  ".byte 0x5b\n"  /* pop ebx  (doesn't exist in 64-bit mode) */
  ".byte 0x59\n"  /* pop ecx  (doesn't exist in 64-bit mode) */
  ".short 0xe8c1\n"  /* shr eax, imm8 */
  ".byte 8\n"
  ".short 0xe083\n"  /* and eax, imm8 */
  ".byte 15\n"
  /* Now EAX contains the CPUID family: 4, 5, 6. */
  "cmp $4, %%al\n"
  "ja cpudetect__5\n"
  "mov $4, %%al\n"
  "jmp cpudetect__pr\n"  /* i486 with CPUID, return 4 in EAX. */
  "cpudetect__5:"  /* >=i586. */
  "cmp $5, %%al\n"
  "jna cpudetect__pr\n"  /* i586 (P5 microarchitecture), return 5 in EAX. */
  /* >=i686 (P6 microarchitecture). */
  "mov $6, %%al\n"
  "cpudetect__pr:"
  ".byte 0x9d\n"  /* 16-bit=(popf); 32-bit=(popfd); 64-bit=(popfq) */
  /* >=i686 (P6 microarchitecture), return 6 in EAX. */
  : "=a" (res));
  return res;
#endif
#endif
}
