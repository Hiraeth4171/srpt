#ifndef _SDOM_H
#define _SDOM_H
typedef struct __attribute__((__packed__)) vec2 {
    int x;
    int y;
} vec2;

typedef struct __attribute__((__packed__)) vec4 {
    int r;
    int g;
    int b;
    int a;
} vec4;

typedef struct __attribute__((__packed__)) Rect {
    vec2 pos;
    vec2 size;
} Rect;

typedef struct Color {
    union {
        unsigned char raw[4];
        struct {
            unsigned char r;
            unsigned char g;
            unsigned char b;
            unsigned char a;
        };
    };
} Color;

typedef enum PropertyType {
    P_COLOR,                // hex color
    P_SIZE,                 // 20 40
    P_POSITION,             // 0,1,2 "absolute, relative, center"
    P_SHOW,                 // 1,0 "true, false"
    P_PADDING,              // left up right down
    P_ORDER,                // 0,1 "vertical|horizontal, v|h"
    P_SRC,                  // "./asset"
    P_EVENT,                // "name: onclick, value: $thing"
    P_CUSTOM,               // anything the user might want
                            // customprop:void*
    P_PLACEHOLDER,          // "string"
    P_SPACE,                // "x y"
    P_CONTENT,              // "string"
} PropertyType;

typedef enum ElementType {
    E_BUTTON,
    E_LABEL,
    E_CANVAS, // this one's gonna need some actual strict definitions
    E_SPACE,  // not sure i actually want this to exist lmao
    E_IMG,
    E_MAIN,
    E_ELEMENT,
    E_TEXTBOX,
    E_SWITCHER,
    E_OVERLAY,
    E_SCROLL,
} ElementType;

typedef struct __attribute__((__packed__)) String {
    unsigned int length;
    char* data;
} String; 

typedef struct __attribute__((__packed__)) Property {
    PropertyType type;
    String name;
    union {
        String value;
        vec2 size;
        vec2 space;
        vec4 padding;
        char position;
        _Bool show;
        _Bool orientation;
        void* custom;
    };
} Property;

struct __attribute__((__packed__)) Element {
    struct Element* _parent;
    ElementType type;
    Rect dim;
    Color color;
    String name;
    unsigned int children_length;
    struct Element** children;
    unsigned int properties_length;
    Property* properties;
};

typedef struct Element Element;

struct __attribute__((__packed__)) Serialized_Element {
    struct Serialized_Element* _parent;
    struct Serialized_Element* children;
    unsigned int children_length;
    ElementType type;
    char* name;
    Rect dim;
    Color color;
    Property* properties;
    unsigned int properties_length;
}; 
typedef struct Serialized_Element Serialized_Element;

typedef struct __attribute__((__packed__)) ScriptSrc {
    unsigned short int len;
    char* script;
} ScriptSrc;

typedef struct __attribute__((__packed__)) Settings {
    vec2 resolution;
    vec2 position;
    String title;
    String method;
    unsigned int scripts_length;
    ScriptSrc* scripts;
    // window flags probably
} Settings;

void append_child(Element* parent, Element* child);
Element* create_element(Element* parent, Element** children, unsigned int length, ElementType type, unsigned int name_length, char* name,  Rect dim, Color color, Property* properties, unsigned int properties_length);
void add_property(Element* elem, PropertyType type, char* name, char** value, unsigned int values_len);


void free_element(Element* elem);
void print_element(Element* elem);

Settings* get_default_settings();
void free_settings(Settings* settings);
void print_settings(Settings* settings);

#endif
