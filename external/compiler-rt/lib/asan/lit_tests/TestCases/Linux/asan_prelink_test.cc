// Test if asan works with prelink.
// It does not actually use prelink, but relies on ld's flag -Ttext-segment
// or gold's flag -Ttext (we try the first flag first, if that fails we
// try the second flag).
//
// RUN: %clangxx_asan -c %s -o %t.o
// RUN: %clangxx_asan -DBUILD_SO=1 -fPIC -shared %s -o %t.so -Wl,-Ttext-segment=0x3600000000 ||\
// RUN: %clangxx_asan -DBUILD_SO=1 -fPIC -shared %s -o %t.so -Wl,-Ttext=0x3600000000
// RUN: %clangxx_asan %t.o %t.so -Wl,-R. -o %t
// RUN: ASAN_OPTIONS=verbosity=1 %t 2>&1 | FileCheck %s

// REQUIRES: x86_64-supported-target, asan-64-bits
#if BUILD_SO
int G;
int *getG() {
  return &G;
}
#else
#include <stdio.h>
extern int *getG();
int main(int argc, char **argv) {
  long p = (long)getG();
  printf("SO mapped at %lx\n", p & ~0xffffffffUL);
  *getG() = 0;
}
#endif
// CHECK: 0x003000000000, 0x004fffffffff{{.*}} MidMem
// CHECK: SO mapped at 3600000000
