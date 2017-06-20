#include <nds.h>
#include <stdio.h>

int main(void)
{
    consoleDemoInit();

    iprintf("PONG");

    while(1)
    {
        swiWaitForVBlank();

        scanKeys();

        int pressed = keysDown();
        if(pressed & KEY_START)
        {
            break;
        }
    }
}
