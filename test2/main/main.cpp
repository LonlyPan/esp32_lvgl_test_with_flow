extern "C" 
{
	#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
}
#include <new>

extern "C" void app_main(void)
{
    while (true) {
        printf("Hello from app_main!\n");
        sleep(1);
    }
}
