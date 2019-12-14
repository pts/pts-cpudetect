pts-cpudetect: library to detect x86 CPU mode and type
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
pts-cpudetect is a library which implements the cpudetect() function in C,
C++ and assembly (NASM) which can detect the CPU mode (16-bit, 32-bit and
64-bit) and CPU type for 32-bit mode (i386, i486, i586 or i686) at runtime.

Implementation files:

* cpudetect.nasm: cpudetect() in NASM syntax.
* cpudetect.s: cpudetect() in GNU assembler (as(1)) syntax.
* cpudetect.c: cpudetect() in C and C++ inline assembly (works in gcc, clang
  and tcc).

Header files:

* cpudetect.h: C and C++ header file for cpudetect(), to be used with
  cpudetect.c.

Demo files:

* cpudetect_main.c: C demo for cpudetect.c or cpudetect.s.

__END__
