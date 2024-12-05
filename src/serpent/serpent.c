#include "./serpent.h"
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

static SDOM_Element* g_main = NULL;

// http://www.cse.yorku.ca/~oz/hash.html#sdbm
size_t hash_function(void* key, size_t size) {
    unsigned long hash = 0;
    int c;
    while((c = *(char*)key++)){
        hash = c + (hash << 6) + (hash << 16) - hash;
    }
    return hash % size;
}
// http://www.cse.yorku.ca/~oz/hash.html#sdbm

int property_cmp(void * p1, void * p2) { // this should honestly still work even if it's not a string
    char* s1 = ((Property*)p1)->name.data, *s2 = (char*)p2; 
    while (*s1 && (*s1 == *s2)) {
        s1++; s2++;
    }
    return *(const unsigned char*)s1 - *(const unsigned char*)s2;
}

SDOM_Element* create_sdom_elem_from_sdom(Element* sdom, SDOM_Element* parent) {
    SDOM_Element* _res = malloc(sizeof(SDOM_Element));
    *_res = (SDOM_Element){
        (String){
            .data=sdom->name.data, 
            .length=sdom->name.length
        },
        sdom->type,
        parent,
        sdom->dim,
        sdom->color,
        sdom->children_length,
        NULL, 
        NULL
    };
    _res->children = malloc(sizeof(SDOM_Element*)*_res->len_children);
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
    g_main = _res;
    return _res;
}

SDOM_Element* srpt_init(Element* sdom) {
    // do some stuff to initialize things idk
    return create_sdom_elem_from_sdom(sdom, NULL);
}

SDOM_Element* get_element_from_parent_by_name(SDOM_Element* parent, char* name) {
    SDOM_Element* _res = NULL;
    for (size_t i = 0; i < parent->len_children; ++i) {
        if(strcmp(parent->children[i]->name.data, name) == 0) {
            return parent->children[i];
        } else {
            _res = get_element_from_parent_by_name(parent->children[i], name);
            if (_res) return _res;
        }
    }
    return _res;
}

SDOM_Element* get_element_by_name(char* name) {
    return get_element_from_parent_by_name(g_main, name);
}

SDOM_Element* get_element_by_string(String* name) {
    return get_element_from_parent_by_name(g_main, name->data);
}

// get elements by name
// get elements by string
