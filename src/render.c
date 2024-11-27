#include <stdio.h>
#include <stdlib.h>
#include "./renderer/renderer.h"
#include "./serpent/serpent.h"

Settings * _settings = NULL;

void read_properties(Property * properties, unsigned int properties_length, FILE* fd) {
    for (unsigned int i = 0; i < properties_length; ++i) {
        fread(&properties[i].type, sizeof(PropertyType), 1, fd);
        fread(&properties[i].name.length, sizeof(unsigned int), 1, fd);
        if (properties[i].name.length > 0) {
            properties[i].name.data = calloc(properties[i].name.length+1, sizeof(char));
            fread(properties[i].name.data, sizeof(char), properties[i].name.length, fd);
        }
        fread(&properties[i].value.length, sizeof(unsigned int), 1, fd);
        if (properties[i].value.length > 0) {
            properties[i].value.data = calloc(properties[i].value.length+1, sizeof(char));
            fread(properties[i].value.data, sizeof(char), properties[i].value.length, fd);
        }
    }
}

void read_element(Element* res, Element* _parent, FILE* fd) {
    // ignore _parent in read, assign after;
    // forward allocate, allocate a child before passing it on;
    // put this all into one fread (smh)
    fread(&res->type, sizeof(ElementType), 1, fd);
    fread(&res->dim, sizeof(Rect), 1, fd);
    fread(&res->color.raw, sizeof(char), 4, fd);
    fread(&res->name.length, sizeof(unsigned int), 1, fd);
    res->name.data = calloc(res->name.length+1, sizeof(char));
    fread(res->name.data, sizeof(char), res->name.length, fd);
    res->name.data[res->name.length] = '\0';
    fread(&res->children_length, sizeof(unsigned int), 1, fd);
    if (res->children_length > 0) res->children = malloc(res->children_length*sizeof(Element*));
    for (unsigned int i = 0; i < res->children_length; ++i) {
        res->children[i] = calloc(1, sizeof(Element));
        read_element(res->children[i], res, fd);
    }
    fread(&res->properties_length, sizeof(unsigned int), 1, fd);
    if(res->properties_length > 0) {
        res->properties = malloc(res->properties_length * sizeof(Property));
        read_properties(res->properties, res->properties_length, fd);
    } else res->properties = NULL;
    res->_parent = _parent;
}

void read_settings(Settings* settings, FILE* fd) {
    // this is terrible fix it
    fread(&settings->resolution, sizeof(vec2), 1, fd);
    fread(&settings->position, sizeof(vec2), 1, fd);
    fread(&settings->title.length, sizeof(unsigned int), 1, fd);
    settings->title.data = calloc(settings->title.length+1, sizeof(char));
    fread(settings->title.data, sizeof(char), settings->title.length, fd);
    settings->title.data[settings->title.length] = '\0';
    fread(&settings->method.length, sizeof(unsigned int), 1, fd);
    settings->method.data = calloc(settings->method.length+1, sizeof(char));
    fread(settings->method.data, sizeof(char), settings->method.length, fd);
    settings->method.data[settings->method.length] = '\0';
    fread(&settings->scripts_length, sizeof(unsigned int), 1, fd);
    if (settings->scripts_length > 0) settings->scripts = calloc(settings->scripts_length, sizeof(ScriptSrc));
    for (unsigned int i = 0; i < settings->scripts_length; ++i) {
        fread(&settings->scripts[i].len, sizeof(unsigned int), 1, fd); 
        settings->scripts[i].script = calloc(settings->scripts[i].len+1, sizeof(char));
        fread(settings->scripts[i].script, sizeof(char), settings->scripts[i].len, fd);
        settings->scripts[i].script[settings->scripts[i].len] = '\0';
    }
}

Element* read_sdom_file (char* sdom_file, _Bool serialized) {
    Element* main = NULL;
    // read binary
    FILE* fd = fopen(sdom_file, "rb");
    if (!serialized) {
        printf("recieved serialized = FALSE\n");
        fread(&main, sizeof(Element*), 1, fd);
        fread(&_settings, sizeof(Settings*), 1, fd);
    } else {
        main = calloc(1, sizeof(Element));
        read_element(main, NULL, fd);
        read_settings(_settings, fd);
    }
    fclose(fd);
    return main;
}


void render(char* sdom_file, _Bool watch, _Bool serialized){
    if (serialized) _settings = calloc(1, sizeof(Settings));
    Element* main = read_sdom_file(sdom_file, serialized);
    print_element(main);
    // 
    SDOM_Element* res = srpt_init(main);  
    renderer_run(res, _settings);
    // ^ event loop
    free_element(main);
    free_settings(_settings);
}

