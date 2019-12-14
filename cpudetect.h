#ifndef __CPUDETECT_H
#define __CPUDETECT_H 1

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
#ifdef __x86_64__  /* Shortcut, also works without it. */
static __inline__ int cpudetect(void) { return 7; }
#else
#ifdef __ia16__  /* Shortcut, also works without it. */
static __inline__ int cpudetect(void) { return 0; }
#else
extern int cpudetect(void);
#endif
#endif

#endif  /* __CPUDETECT_H */
