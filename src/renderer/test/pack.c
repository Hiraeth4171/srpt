#include <serpent.h>
#include <stdio.h>

unsigned int counter = 0;

extern void silly () {
    printf("%d\n", counter++);
}

int main (void) {
    srpt_init(NULL);
    return 0;
}

