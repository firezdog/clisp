#include<stdio.h>

// the problem
void invoke_callback(int range, void (*callback_for_int) (int)) 
{
    for (int i = 0; i < range; i++)
    {
        callback_for_int(i);
    }
}

void callback(int x)
{
    printf("%d\n", x);
}

void main()
{
    invoke_callback(10, callback);
}