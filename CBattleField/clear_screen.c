#include <windows.h>
#include "clear_screen.h"

void SetPos( int x, int y ) {
    COORD point = { x, y };
    HANDLE HOutput = GetStdHandle( STD_OUTPUT_HANDLE );

    SetConsoleCursorPosition( HOutput, point );
}