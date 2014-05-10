#include <stdio.h>
#include "serial.h"
#include "s3c24xx.h"

int main()
{
    char c;
    
    uart0_init();
    
    printf("qqqwwqw\n");
    printf("int:%d\n",11);

    while (1)
    {}

    return 0;
}
