#include <stdio.h>

// Normal pointer functions
void f(int *p)
{
    *p = 42;
}
int g()
{
    int x = 1;
    int *p = &x;
    f(p);
    return x;
}
void normal_pointer()
{
    printf("%d\n", g());
}

// Pointer to pointer functions
void pointer_to_pointer()
{
    int x = 1;
    int *p = &x;
    int **q = &p;
    **q = 42;
    printf("%d\n", x);
}

int main()
{
    normal_pointer();
    pointer_to_pointer();
    return 0;
}
