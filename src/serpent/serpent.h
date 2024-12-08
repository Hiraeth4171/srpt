#ifndef _SERPENT_H
#define _SERPENT_H
#include "../sdom.h"
#include <SDT/sdt.h>

typedef struct SDOM_Element SDOM_Element;

struct SDOM_Element {
    String name;
    ElementType type;
    SDOM_Element* parent;
    Rect dim;
    Rect actual_dim;
    Color bg_color;
    unsigned int len_children;
    SDOM_Element** children;
    SDTHashtable* properties;
};

SDOM_Element* srpt_init(Element* sdom);
Element* srpt_read_bytearray_sdom(void** ptr, Element* parent);
Settings* srpt_read_bytearray_settings(void** ptr);
SDOM_Element* get_element_by_name(char*);
SDOM_Element* get_element_by_string(String*);
void update_properties(SDOM_Element* elem);
#endif
