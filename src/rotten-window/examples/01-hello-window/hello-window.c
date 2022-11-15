#include <stdio.h>
#include "rotten-window.h"

int main()
{
    rotten_window window;
    rotten_success_code err = rotten_window_connect(&window);

    return 0;
}
