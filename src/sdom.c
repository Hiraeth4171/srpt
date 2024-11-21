#include "./sdom.h"
#include "./utils.h"
#include <stdio.h>
#include <stdlib.h>


Element* create_element(Element* parent, Element** children,unsigned int length, ElementType type, unsigned int name_length, char* name, Rect dim, Color color, Property* properties, unsigned int properties_length) {
    Element* _res = calloc(1, sizeof(Element));
    _res->_parent = parent;
    _res->children = children;
    _res->type = type;
    _res->name.length = name_length;
    _res->name.data = name;
    _res->children_length = length;
    _res->dim = dim;
    _res->color = color;
    _res->properties = properties;
    _res->properties_length = properties_length;
    return _res;
}

void append_child(Element* parent, Element* child) {
    parent->children = (Element**)realloc(parent->children, (parent->children_length+1)*sizeof(Element*));
    parent->children[parent->children_length++] = child;
}

void add_property(Element* elem, PropertyType type, char* name, char* value) {
    elem->properties = realloc(elem->properties, (elem->properties_length+1)*sizeof(Property));
    unsigned int name_len = str_len(name), value_len = str_len(value);
    elem->properties[elem->properties_length].type = type;
    elem->properties[elem->properties_length].name.length = name_len;
    elem->properties[elem->properties_length].name.data = create_str(name, name_len);
    elem->properties[elem->properties_length].value.length = value_len;
    elem->properties[elem->properties_length++].value.data = create_str(value, value_len);
}

Settings* get_default_settings() {
    Settings* settings = calloc(1,sizeof(Settings));
    settings->title.length = 19;
    settings->title.data = create_str("Serpent Application", 19);
    settings->resolution = (vec2){1920,1080};
    settings->position = (vec2){0,0};
    settings->method.length = 17;
    settings->method.data = create_str("interpret,python", 17);
    settings->scripts_length = 0;
    settings->scripts = NULL;
    return settings;
}

void free_properties(Property* properties, unsigned int properties_length) {
    for (int i = 0; i < properties_length; ++i) {
        free(properties[i].name.data);
        free(properties[i].value.data);
    }
    free(properties);
}

void free_element(Element* elem) {
    for(int i = 0; i < elem->children_length; ++i) {
        free_element(elem->children[i]);
    }
    free(elem->name.data);
    if (elem->children != NULL) free(elem->children);
    if (elem->properties != 0) free_properties(elem->properties, elem->properties_length);
    free(elem);
}


void free_settings(Settings* settings) {
    free(settings->title.data);
    free(settings->method.data);
    for (unsigned int i = 0; i < settings->scripts_length; ++i) free(settings->scripts[i].script);
    free(settings->scripts);
    free(settings);
}

void print_settings(Settings* settings) {
    printf("\n\n\n Settings:\n");
    printf("\t\t resolution: x: %d, y: %d\n", settings->resolution.x, settings->resolution.y);
    printf("\t\t title: %s\n", settings->title.data);
    printf("\t\t scripts:\n");
    //for (unsigned int i = 0; i < settings->scripts_length; ++i) printf("\t\t\t %s\n", settings->scripts[i].script);
    printf("\t\t method: %s\n", settings->method.data);
}

void print_properties(Property* properties, unsigned int length) {
    if (properties == NULL) return;
    for (unsigned int i = 0; i < length; ++i) {
        printf("\n\t{%s, %s}\n", properties[i].name.data, properties[i].value.data);
    }
}

void print_element(Element* elem) {
    printf("( RECT: %d, %d, %d, %d; ", elem->dim.pos.x, elem->dim.pos.y, elem->dim.size.y, elem->dim.size.y);
    printf("COLOR: %d, %d, %d, %d; ", elem->color.r, elem->color.g, elem->color.b, elem->color.a);
    printf("TYPE: %d; ", elem->type);
    printf("NAME: %s; ", elem->name.data);
    printf("PROPERTIES: ");
    print_properties(elem->properties, elem->properties_length);
    if(elem->_parent != NULL) printf("_PARENT_NAME: %s; ", elem->_parent->name.data);
    printf("CHILDREN: %u ) ", elem->children_length);
    for(int i = 0; i < elem->children_length; ++i) {
        if(i == 0) printf("{\n\n\t");
        print_element(elem->children[i]);
        printf("\n\t");
        if (i == elem->children_length-1) printf("\n\n}");
    }
}

