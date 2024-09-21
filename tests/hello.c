#include <stdio.h>
char message[] = "welcome to the OS, OS name is Quantum. Build 10001\n";
char buffer[1024]; // .bss
int main(void)
{
    printf(message);
    getchar();
    return 0;
}
