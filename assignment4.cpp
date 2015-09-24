#include <stdlib.h>

int main() {
        asm(
        "xor %eax, %eax\n"
        "push $0x0068732f\n"
        "push $0x6e69622f\n"
        "mov %esp, %ebx\n"
        "push %eax\n"
        "push %eax\n"
        "push %ebx\n"
        "mov %esp, %ecx\n"
        "mov $0xb, %al\n"
        "xor %edx, %edx\n"
        "push %ecx\n"
        "push %edx\n"
        "push %ebp\n"
        "mov %esp, %ebp\n"
        "sysenter\n"
        "int $0x80\n");
        return 0;
}
