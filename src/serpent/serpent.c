#include "./serpent.h"
#include <stddef.h>
#include <stdlib.h>

// http://www.cse.yorku.ca/~oz/hash.html#sdbm
size_t hash_function(void* key, size_t size) {
    unsigned long hash = 0;
    int c;
    while((c = *(char*)key++))
        hash = c + (hash << 6) + (hash << 16) - hash;

    return hash % size;
}
// http://www.cse.yorku.ca/~oz/hash.html#sdbm

int property_cmp(void * p1, void * p2) {
    char* s1 = ((Property*)p1)->name.data, *s2 = (char*)p2;
    while (*s1 && (*s1 == *s2)) {
        s1++; s2++;
    }
    return *(const unsigned char*)s1 - *(const unsigned char*)s2;
}

SDOM_Element* create_sdom_elem_from_sdom(Element* sdom, SDOM_Element* parent) {
    SDOM_Element* _res = malloc(sizeof(SDOM_Element));
    _res->dim.pos = sdom->dim.pos;
    _res->dim.size = sdom->dim.size;
    _res->name = (String){.data=sdom->name.data, .length=sdom->name.length};
    _res->type = sdom->type;
    _res->bg_color = sdom->color;
    _res->len_children = sdom->children_length;
    _res->children = malloc(sizeof(SDOM_Element*)*_res->len_children);
    _res->parent = parent;
    for (size_t i = 0; i < sdom->children_length; ++i) {
        _res->children[i] = create_sdom_elem_from_sdom(sdom->children[i], _res);
    }
    if (sdom->properties_length == 0) {
        _res->properties = NULL;
        return _res;
    }
    _res->properties = sdt_hashtable_init(sdom->properties_length, sizeof(Property), hash_function, property_cmp, NULL);
    for (size_t i = 0; i < sdom->properties_length; ++i) {
        sdt_hashtable_add(_res->properties, (void*)&sdom->properties[i]);
    }
    return _res;
}

SDOM_Element* srpt_init(Element* sdom) {
    // do some stuff to initialize things idk
    return create_sdom_elem_from_sdom(sdom, NULL);
}
