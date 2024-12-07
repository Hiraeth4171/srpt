#include "./serpent.h"
#include "SDT/sdt.h"
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

static SDOM_Element** g_main = NULL;

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


void print_property(void* prop) {
    Property* propert = (Property*)prop;
    Property property = *propert;
    static const char* pos_lookup[3] =(const char*[3]){"absolute", "relative", "center"};
    static const char* orientation_lookup[2] =(const char*[2]){"vertical", "horizontal"};
    static const char* true_false_lookup[2] =(const char*[2]){"false", "true"};
    switch (property.type) {
        case P_COLOR: case P_PLACEHOLDER: case P_CONTENT: case P_SRC: case P_EVENT: 
            printf("\n\t{%s, %s}\n", property.name.data, property.value.data);
            break;
        case P_SIZE: case P_SPACE: 
            printf("\n\t{%s, vec2[%d,%d]}\n", property.name.data, property.space.x, property.space.x);
            break;
        case P_POSITION:
            printf("\n\t{%s, %s}\n", property.name.data, pos_lookup[(int)property.position]);
            break;
        case P_ORDER:
            printf("\n\t{%s, %s}\n", property.name.data, orientation_lookup[(int)property.orientation]);
        case P_SHOW:
            printf("\n\t{%s, %s}\n", property.name.data, true_false_lookup[(int)property.show]);
            break;
        case P_PADDING:
            printf("\n\t{%s, vec4[%d,%d,%d,%d]}\n", 
                    property.name.data, 
                    property.padding.r, 
                    property.padding.g,
                    property.padding.b,
                    property.padding.a);
            break;
        case P_CUSTOM:
            break;
    }
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
    _res->children = malloc(sizeof(SDOM_Element*)*(_res->len_children));
    for (size_t i = 0; i < sdom->children_length; ++i) {
        _res->children[i] = create_sdom_elem_from_sdom(sdom->children[i], _res);
    }
    if (sdom->properties_length == 0) {
        _res->properties = NULL;
    } else { 
        _res->properties = sdt_hashtable_init(sdom->properties_length, sizeof(Property), hash_function, property_cmp, NULL);
    }
    for (size_t i = 0; i < sdom->properties_length; ++i) {
        sdt_hashtable_add(_res->properties, (void*)&sdom->properties[i].name.data, (void*)&sdom->properties[i]);
        printf("%s\n", sdom->name.data);
        print_property(&sdom->properties[i]);
    }
    if (parent == NULL) { 
        *g_main = _res;
    }
    return _res;
}

SDOM_Element* srpt_init(Element* sdom) {
    // do some stuff to initialize things idk
    g_main = malloc(sizeof(SDOM_Element*));
    create_sdom_elem_from_sdom(sdom, NULL);
    if ((*g_main)->properties != NULL) {
        sdt_hashtable_print((*g_main)->properties, print_property);
    }
    return *g_main;
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
    return get_element_from_parent_by_name(*g_main, name);
}

SDOM_Element* get_element_by_string(String* name) {
    return get_element_from_parent_by_name(*g_main, name->data);
}

// get elements by name
// get elements by string
