#include "./serpent.h"
#include "../renderer/renderer.h"
#include "SDT/sdt.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static SDOM_Element** g_main = NULL;

#ifdef SRPT_PACKAGED
static Settings** g_srpt_settings = NULL;
#define mem_read(dst, src, len)\
    memcpy((dst), (src), (len));\
    (src)+=(len);

// get sdom from the binary directly
extern const char sdom[];
extern const char* sdom_end;
extern int sdom_size;
#else
#define mem_read(dst, src, len)\
    printf("failure.");
#endif


void read_props(void** ptr, Property* props, unsigned int props_length) {
    for (unsigned int i = 0; i < props_length; ++i) {
        mem_read(&props[i].type, *ptr, sizeof(PropertyType));
        mem_read(&props[i].name.length, *ptr, sizeof(unsigned int));
        if (props[i].name.length > 0) {
            props[i].name.data = calloc(props[i].name.length+1, sizeof(char));
            mem_read(props[i].name.data, *ptr, props[i].name.length);
        }
        switch (props[i].type) {
            case P_COLOR:
                goto string;
                break;
            case P_SIZE:
                mem_read(&props[i].size, *ptr, sizeof(vec2));
                break;
            case P_POSITION:
                mem_read(&props[i].position, *ptr, 1);
                break;
            case P_PADDING:
                mem_read(&props[i].padding, *ptr, sizeof(vec4));
                break;
            case P_ORDER:
                mem_read(&props[i].orientation, *ptr, 1);
            case P_SHOW:
                mem_read(&props[i].show, *ptr, sizeof(_Bool));
                break;
            case P_ONCLICK:
                goto string;
                break;
            case P_SRC:
                goto string;
                break;
            case P_EVENT:
                goto string;
                break;
            case P_CUSTOM:
                // no clue yet
                goto string; // for now
                break;
            case P_PLACEHOLDER:
                goto string;
                break;
            case P_CONTENT:
                goto string;
                break;
            case P_SPACE:
                mem_read(&props[i].space, *ptr, sizeof(vec2));
                break;
        }
        continue;
string:
        mem_read(&props[i].value.length, *ptr, sizeof(unsigned int));
        if (props[i].value.length > 0) {
            props[i].value.data = calloc(props[i].value.length+1, sizeof(char));
            mem_read(props[i].value.data, *ptr, props[i].value.length);
        }
    }
}

Element* srpt_read_bytearray_sdom(void** ptr, Element* parent) {
    Element* _res = malloc(sizeof(Element));
    mem_read(&_res->type, *ptr, sizeof(ElementType) + sizeof(Rect) + sizeof(Color));
    mem_read(&_res->name.length, *ptr, sizeof(unsigned int));
    _res->name.data = malloc(sizeof(_res->name.length));
    mem_read(_res->name.data, *ptr, _res->name.length);
    mem_read(&_res->children_length, *ptr, sizeof(unsigned int));
    if (_res->children_length > 0) _res->children = malloc(sizeof(Element*)*_res->children_length);
    for (unsigned int i = 0; i < _res->children_length; ++i) {
        _res->children[i] = srpt_read_bytearray_sdom(ptr, _res);
    }
    mem_read(&_res->properties_length, *ptr, sizeof(unsigned int));
    if (_res->properties_length > 0) {
        _res->properties = malloc(sizeof(Property)*_res->properties_length);
        read_props(ptr, _res->properties, _res->properties_length);
    } else _res->properties = NULL;
    return _res;
}

Settings* srpt_read_bytearray_settings(void** ptr) {
    Settings* settings = malloc(sizeof(Settings));
    mem_read(&settings->resolution, *ptr, sizeof(vec2));
    mem_read(&settings->position, *ptr, sizeof(vec2));
    mem_read(&settings->title.length, *ptr, sizeof(unsigned int));
    settings->title.data = malloc(sizeof(settings->title.length));
    mem_read(settings->title.data, *ptr, settings->title.length); 
    mem_read(&settings->method.length, *ptr, sizeof(unsigned int)); 
    settings->method.data = malloc(sizeof(settings->method.length));
    mem_read(settings->method.data, *ptr, settings->method.length); 
    mem_read(&settings->scripts_length, *ptr, sizeof(unsigned int)); 
    if (settings->scripts_length > 0) settings->scripts = malloc(settings->scripts_length*sizeof(ScriptSrc));
    for (unsigned int i = 0; i < settings->scripts_length; ++i) {
        mem_read(&settings->scripts[i].len, *ptr, sizeof(unsigned short int));
        mem_read(settings->scripts[i].script, *ptr, settings->scripts[i].len);
    }
    return settings;
}



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
        case P_COLOR: case P_PLACEHOLDER: case P_CONTENT: case P_SRC: case P_EVENT: case P_ONCLICK: 
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
        (Rect){(vec2){sdom->dim.pos.x, sdom->dim.pos.y}, (vec2){sdom->dim.size.x, sdom->dim.size.y}},
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
    printf("%s\n", sdom->name.data);
    for (size_t i = 0; i < sdom->properties_length; ++i) {
        sdt_hashtable_add(_res->properties, (void*)sdom->properties[i].name.data, (void*)&sdom->properties[i]);
        print_property(&sdom->properties[i]);
    }
    if (parent == NULL) { 
        *g_main = _res;
    }
    return _res;
}

SDOM_Element* srpt_init(Element* _sdom) {
    // do some stuff to initialize things idk
    g_main = malloc(sizeof(SDOM_Element*));
    if (_sdom == NULL) {
    #ifdef SRPT_PACKAGED
        //unsigned long offset = (sdom[3] << 24) | (sdom[2] << 16) | (sdom[1] << 8) | sdom[0];
        void* ptr = (void*)sdom+8;
        g_srpt_settings = malloc(sizeof(Settings*));
        *g_srpt_settings = srpt_read_bytearray_settings(&ptr);    
        _sdom = srpt_read_bytearray_sdom(&ptr, NULL);
    #else
        printf("<srpt_init> no sdom Element specificed");
    #endif
    }
    create_sdom_elem_from_sdom(_sdom, NULL);
    if ((*g_main)->properties != NULL) {
        sdt_hashtable_print((*g_main)->properties, print_property);
    }
    #ifdef SRPT_PACKAGED
        renderer_run(*g_main, *g_srpt_settings);
    #endif
    return *g_main;
}

void srpt_set_property (SDOM_Element* elem, char* property_name, void* value) {
    Property* prop;
    if ((prop = sdt_hashtable_get(elem->properties, property_name)) != NULL) {
        switch (prop->type) {
            case P_POSITION:
                prop->position = *(char*)value;
                break;
            case P_SHOW:
                prop->show = *(_Bool*)value;
                break;
            case P_ONCLICK:
                prop->value.length = strlen((char*)value);
                prop->value.data = (char*)value;
                break;
            default: // yk add the rest later and allat
                break;
        }
    } else { // need to create a new property entry;
        // do this later i cba rn
    }
    update_properties(elem); // probs wanna make an update_property() to use 
                             // here instead and put that into the switch statement
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

SDOM_Element* srpt_get_element_by_name(char* name) {
    return get_element_from_parent_by_name(*g_main, name);
}

SDOM_Element* srpt_get_element_by_string(String* name) {
    return get_element_from_parent_by_name(*g_main, name->data);
}

SDOM_Element* srpt_create_element() {
    // later
    return NULL;
}

void update_properties(SDOM_Element* elem) {
    // this is temporary
    Property* tmp;
    if ((tmp = (Property*)sdt_hashtable_get(elem->properties, "position")) != NULL) {
        switch(tmp->position) {
            case 0:
                elem->actual_dim.pos.x = elem->parent->dim.pos.x;
                elem->actual_dim.pos.y = elem->parent->dim.pos.y;
                break;
            case 1:
                elem->actual_dim.pos.x += elem->parent->dim.pos.x;
                elem->actual_dim.pos.y += elem->parent->dim.pos.y;
                break;
            case 2:
                elem->actual_dim.pos.x += elem->parent->dim.pos.x + elem->parent->dim.size.x/2 - elem->dim.size.x/2;
                elem->actual_dim.pos.y += elem->parent->dim.pos.y + elem->parent->dim.size.y/2 - elem->dim.size.y/2;
                break;
        }
    }
    if ((tmp = (Property*)sdt_hashtable_get(elem->properties, "show")) != NULL) {
        if(!tmp->show) {
            // store something in SDOM_Element that says if it's visible or not
            // and just change that ig
        }
    }
    for (size_t i = 0; i < elem->len_children; ++i) {
        update_properties(elem->children[i]);
    }
}
