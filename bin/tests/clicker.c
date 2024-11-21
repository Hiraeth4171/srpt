#include "serpent.h"

unsigned long count;
char* buffer;

int main() {
    srpt_init();
    Element main = srpt_get_main();
    main.children[0].content = "0";
    buffer = malloc(11);
}

void count() {
    sprintf(buffer, "%ul", ++count);
    main.children[0].content = buffer;
}
