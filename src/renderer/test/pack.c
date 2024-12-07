#include "../renderer.h"
#include "../../serpent/serpent.h"
#include "../../sdom.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define mem_read(dst, src, len)\
    memcpy((dst), (src), (len));\
    (src)+=(len);

// get sdom from the binary directly
extern const char sdom[];
extern const char* sdom_end;
extern int sdom_size;

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

Element* read_elem(void** ptr, Element* parent) {
    Element* _res = malloc(sizeof(Element));
    mem_read(&_res->type, *ptr, sizeof(ElementType) + sizeof(Rect) + sizeof(Color));
    mem_read(&_res->name.length, *ptr, sizeof(unsigned int));
    _res->name.data = malloc(sizeof(_res->name.length));
    mem_read(_res->name.data, *ptr, _res->name.length);
    mem_read(&_res->children_length, *ptr, sizeof(unsigned int));
    if (_res->children_length > 0) _res->children = malloc(sizeof(Element*)*_res->children_length);
    for (unsigned int i = 0; i < _res->children_length; ++i) {
        _res->children[i] = read_elem(ptr, _res);
    }
    mem_read(&_res->properties_length, *ptr, sizeof(unsigned int));
    if (_res->properties_length > 0) {
        _res->properties = malloc(sizeof(Property)*_res->properties_length);
        read_props(ptr, _res->properties, _res->properties_length);
    } else _res->properties = NULL;
    return _res;
}

int main (void) {
    unsigned long offset = (sdom[3] << 24) | (sdom[2] << 16) | (sdom[1] << 8) | sdom[0];
    printf("%lu\n",offset);
    void* ptr = (void*)sdom+8;
    Settings* settings = malloc(sizeof(Settings));

    mem_read(&settings->resolution, ptr, sizeof(vec2));
    mem_read(&settings->position, ptr, sizeof(vec2));
    mem_read(&settings->title.length, ptr, sizeof(unsigned int));
    settings->title.data = malloc(sizeof(settings->title.length));
    mem_read(settings->title.data, ptr, settings->title.length); 
    mem_read(&settings->method.length, ptr, sizeof(unsigned int)); 
    settings->method.data = malloc(sizeof(settings->method.length));
    mem_read(settings->method.data, ptr, settings->method.length); 
    mem_read(&settings->scripts_length, ptr, sizeof(unsigned int)); 
    if (settings->scripts_length > 0) settings->scripts = malloc(settings->scripts_length*sizeof(ScriptSrc));
    for (unsigned int i = 0; i < settings->scripts_length; ++i) {
        mem_read(&settings->scripts[i].len, ptr, sizeof(unsigned short int));
        mem_read(settings->scripts[i].script, ptr, settings->scripts[i].len);
    }
    // settings read.
    
    Element* elem = read_elem(&ptr, NULL);
    renderer_run(srpt_init(elem), settings);
    return 0;
}
