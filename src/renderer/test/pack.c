#include <serpent.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

unsigned int counter = 0;
SDOM_Element* g_sdom_main = NULL;

extern void silly () {
    SDOM_Element* clicker = get_element_by_name("counter");
    if (clicker) clicker->bg_color = (Color){.r=rand()%255, .g=rand()%255, .b=rand()%255, .a=rand()%255};
    Property* tmp;
    if ((tmp = sdt_hashtable_get(clicker->properties, "position")) != NULL) {
        tmp->position = (counter++)%3;
        printf("%d\n", tmp->position);
    }
    update_properties(clicker);
}

int main (void) {
    srand(time(NULL));
    g_sdom_main = srpt_init(NULL);
    return 0;
}

