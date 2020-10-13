#include <stdio.h>

#define forever while(1)

int main() {
    int i = 0;
    forever printf("%i\n", i++);
    return 0;
}
