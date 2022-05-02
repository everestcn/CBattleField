#include <time.h>

int Delay( int milliseconds )
{
    int start = clock();
    while( clock() - start < milliseconds );
    return 0;
}
