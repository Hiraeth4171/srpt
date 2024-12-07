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

void add_property(Element* elem, PropertyType type, char* name, char** values, unsigned int values_len) {
    elem->properties = realloc(elem->properties, (elem->properties_length+1)*sizeof(Property)); // this is a bit insane
    elem->properties[elem->properties_length].type = type;
    unsigned int name_len = str_len(name);
    elem->properties[elem->properties_length].name.length = name_len;
    elem->properties[elem->properties_length].name.data = create_str(name, name_len);
    switch (type) {
        case P_COLOR:
            if (values_len != 1) {
                printf("BROKEN."); // later support rgba(stuff) or something
            } else goto string;
            break;
        case P_SIZE:
            if (values_len != 2) printf("BROKEN.");
            elem->properties[elem->properties_length].size = (vec2){ atoi(values[0]), atoi(values[1]) };
            break;
        case P_POSITION:
            if (values_len == 1) { 
                if (values[0][0] == 'a') elem->properties[elem->properties_length].position = 0;
                else if (values[0][0] == 'r') elem->properties[elem->properties_length].position = 1;
                else if (values[0][0] == 'c') elem->properties[elem->properties_length].position = 2;
            }
            break;
        case P_SHOW:
            if (values_len == 1) { 
                if (values[0][0] == 't') elem->properties[elem->properties_length].show = 1;
                else if (values[0][0] == 'f') elem->properties[elem->properties_length].show = 0;
            }
            break;
        case P_PADDING:
            if (values_len == 2) elem->properties[elem->properties_length].padding = (vec4){ atoi(values[0]), atoi(values[1]), atoi(values[0]), atoi(values[1]) };
            if (values_len == 4) elem->properties[elem->properties_length].padding = (vec4){ atoi(values[0]), atoi(values[1]), atoi(values[2]), atoi(values[3]) };
            break;
        case P_ORDER:
            if (values_len == 1) elem->properties[elem->properties_length].orientation = str_cmp("vertical", values[0]) == 0 ? 0 : 1;
            break;
        case P_SRC:
            goto string;
            break;
        case P_EVENT:
            goto string;
            break;
        case P_CUSTOM:
            switch (values_len) {
                case 1:
                    goto string;
                    break;
                case 2:
                    vec2* v2 = malloc(sizeof(vec2));
                    *v2 = (vec2){ atoi(values[0]), atoi(values[1]) };
                    elem->properties[elem->properties_length].custom = (void*)v2;
                case 4:
                    vec4* v4 = malloc(sizeof(vec2));
                    *v4 = (vec4){ atoi(values[0]), atoi(values[1]), atoi(values[2]), atoi(values[3]) };
                    elem->properties[elem->properties_length].custom = (void*)v4;
            }
            break;
        case P_PLACEHOLDER:
            goto string;
            break;
        case P_CONTENT:
            goto string;
            break;
        case P_SPACE:
            if (values_len != 2) printf("BROKEN.");
            elem->properties[elem->properties_length].space = (vec2){ atoi(values[0]), atoi(values[1]) };
            break;
    }
    elem->properties_length++;
    return;
string:
    unsigned int value_len = str_len(values[0]);
    elem->properties[elem->properties_length].value.length = value_len;
    elem->properties[elem->properties_length++].value.data = create_str(values[0], value_len);
    return;
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
        switch (properties[i].type) {
            case P_SHOW: case P_POSITION:
                break;
            default:
                free(properties[i].value.data); // VERY TEMPORARY
                break;
        }
    }
    free(properties);
}

void free_element(Element* elem) {
    for(int i = 0; i < elem->children_length; ++i) {
        free_element(elem->children[i]);
    }
    free(elem->name.data);
    if (elem->children != NULL) free(elem->children);
    if (elem->properties_length != 0) free_properties(elem->properties, elem->properties_length);
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
    printf("length: %d\n", length);
    if (properties == NULL) return;
    static const char* pos_lookup[3] =(const char*[3]){"absolute", "relative", "center"};
    static const char* orientation_lookup[2] =(const char*[2]){"vertical", "horizontal"};
    static const char* true_false_lookup[2] =(const char*[2]){"false", "true"};
    for (unsigned int i = 0; i < length; ++i) {
        switch (properties[i].type) {
            case P_COLOR: case P_PLACEHOLDER: case P_CONTENT: case P_SRC: case P_EVENT: 
                printf("\n\t{%s, %s}\n", properties[i].name.data, properties[i].value.data);
                break;
            case P_SIZE: case P_SPACE: 
                printf("\n\t{%s, vec2[%d,%d]}\n", properties[i].name.data, properties[i].space.x, properties[i].space.x);
                break;
            case P_POSITION:
                printf("\n\t{%s, %s}\n", properties[i].name.data, pos_lookup[(int)properties[i].position]);
                break;
            case P_ORDER:
                printf("\n\t{%s, %s}\n", properties[i].name.data, orientation_lookup[(int)properties[i].orientation]);
            case P_SHOW:
                printf("correct!\n");
                printf("\n\t{%s, %s}\n", properties[i].name.data, true_false_lookup[(int)properties[i].show]);
                break;
            case P_PADDING:
                printf("\n\t{%s, vec4[%d,%d,%d,%d]}\n", 
                        properties[i].name.data, 
                        properties[i].padding.r, 
                        properties[i].padding.g,
                        properties[i].padding.b,
                        properties[i].padding.a);
                break;
            case P_CUSTOM:
                break;
        }
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

