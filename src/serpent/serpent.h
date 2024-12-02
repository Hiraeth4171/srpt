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
    Color bg_color;
    unsigned int len_children;
    SDOM_Element** children;
    SDTHashtable* properties;
};

SDOM_Element* srpt_init(Element* sdom);
SDOM_Element* get_element_by_name(char*);
SDOM_Element* get_element_by_string(String*);
#endif
