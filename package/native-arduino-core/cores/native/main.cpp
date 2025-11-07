#include "Arduino.h"

int main(int argc, char **argv)
{
    // Arduino スタイルの呼び出し
    setup();
    for (;;)
    {
        loop();
    }
    return 0;
}
