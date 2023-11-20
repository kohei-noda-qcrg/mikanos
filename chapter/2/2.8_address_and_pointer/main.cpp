#include <stdio.h>
#include <cstdint>

int main()
{
    int i = 42;
    int *p = &i;
    printf("i = %d\n", i); // i = 42
    // print p (address of i)
    printf("p = %p\n", p); // p = address of i
    // print *p (value of i)
    printf("*p = %d\n", *p); // *p = 42

    int r1 = *p;             // r1 = *p = i = 42
    *p = 1;                  // *p = i = 1
    int r2 = i;              // r2 = i = 1
    printf("r1 = %d\n", r1); // r1 = 42
    printf("r2 = %d\n", r2); // r2 = 1
    printf("*p = %d\n", *p); // *p = 1
    printf("i = %d\n", i);   // i = 1

    // - ポインタの実現方法についてはC++ではほとんど規定されていない
    //   - ただし、ポインタと整数を相互変換することはできる
    //   - 異なる整数へのポインタを整数にキャストしたものは、互いに異なる整数になる
    //     - この制約の実現方法は、処理系に任されている
    //     - 多くのコンパイラでは、メモリアドレスをキャスト時に得られる整数として使うことで一意な整数を得ている
    uintptr_t addr = reinterpret_cast<uintptr_t>(p); // Cast the p address to uintptr_t, this process is corresponding to removing the type information of p
    int *q = reinterpret_cast<int *>(addr);          // Cast from an uintptr_t to an int*, p is originally pointing to int i, so q also points to i
};
