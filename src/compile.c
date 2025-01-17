#include <stdio.h>
#include <stdlib.h>
#include "./compile.h"
#include "./utils.h"
#include "./sdom.h"

#define DEBUG

#define CHAR_AT_START(CHAR,PTR,BUFFER) (*(PTR) == CHAR && (PTR) == (BUFFER)) || (*(PTR) == CHAR && *((PTR)-1) == '\n') 
#define ONE_CHAR_TOKEN(CHAR, STR, TOKENTYPE, PTR, LEN, TOKENS, INDEX)\
    else if (*(PTR) == (CHAR)) {\
            *(LEN) = 1;\
            create_token_at_index(TOKENS, INDEX, TOKENTYPE, STR, LEN);\
            (INDEX)++;\
    }

#define TOKENIZE_KEYWORD(KEYWORD, TOKENTYPE, PTR, LEN, TOKENS, INDEX)\
    else if (live_compare((KEYWORD), (PTR), (LEN))) {\
        create_token_at_index(TOKENS, INDEX,\
                TOKENTYPE, KEYWORD, LEN);\
        (PTR)+=*(LEN)-1;(INDEX)++;\
    }

#define TOKENIZE_DYNAMIC_KEYWORD(STR, CONDITION, TOKENTYPE, PTR, LEN, TOKENS, INDEX)\
    else if (CONDITION) {\
        create_token_at_index(TOKENS, INDEX,\
                TOKENTYPE, STR, LEN);\
        (PTR)+=*(LEN)-1;(INDEX)++;\
    }

#define ASSERT(THING) if(!(THING)) exit(1);

typedef enum {
    LBRACE = 0, // {
    RBRACE, // }
    COL, // :
    SEMI, // ;
    COLOR, // color
    BACKGROUND_COLOR, //BACKGROUND_COLOR
    ELEM_LOOKUP,
    ROTATE,
    ONCLICK,
    IMG,
    BUTTON,
    DIM,
    TEXTBOX,
    CANVAS,
    SWITCHER,
    OVERLAY,
    CONTENT,
    BANG,
    VAR_LOOKUP, // $thing
    POSITION,
    SHOW,
    DOT, // .
    LADDER, // #
    HEX_COLOR, // #098340
    RGB_COLOR, // rgb(), RGB()
    RGBA_COLOR, // rgb(), RGB()
    NUMBER,
    STRING,
} Token_Type;

#ifdef DEBUG
char* lookup[] = {
    "LBRACE",
    "RBRACE",
    "COL",
    "SEMI",
    "COLOR",
    "BACKGROUND_COLOR",
    "ELEM_LOOKUP",
    "ROTATE",
    "ONCLICK",
    "IMG", 
    "BUTTON",
    "DIM",
    "TEXTBOX",
    "CANVAS",
    "SWITCHER",
    "OVERLAY",
    "CONTENT",
    "BANG (!)",
    "VAR_LOOKUP",
    "POSITION",
    "SHOW",
    "DOT",
    "LADDER",
    "HEX_COLOR",
    "RGB_COLOR",
    "RGBA_COLOR",
    "NUMBER",
    "STRING",
};
#endif

typedef struct {
    Token_Type type;
    unsigned int length;
    char* value;
} Token;


Settings* l_settings = NULL;

char* lex(char* buffer, long size);
Token* tokenize(char* buffer, long* size);
void print_tokens(Token* tokens, long len);
void free_tokens(Token* tokens, long len);
Element* sdom(Token* tokens, long len);
void write_sdom_to_binary(Element* res, char* output_file, bool serialize);


int compile(char* filename, char* output, bool watch, bool serialize) {
    const char* file = filename;
    long* len = malloc(sizeof(long));
    char* buffer = read_file(file, len);
    char* lexed = lex(buffer, *len);
    free(buffer);
    /*printf("\n--------------------------------------------\n");*/
    /*printf("\n\n\t (**) %s\n\n\n", lexed);*/
    /*printf("\n--------------------------------------------\n");*/
    Token* tokens = tokenize(lexed, len);
    free(lexed);
    print_tokens(tokens, *len);
    Element* res = sdom(tokens, *len);
    free_tokens(tokens, *len);
    free(len);
    if (output == NULL) write_sdom_to_binary(res, "output.sdom", serialize);
    else write_sdom_to_binary(res, output, serialize);
    if (watch) {
        // include code for starting a thread to watch for changes in filename's last edited date
    }
    if (serialize) {
        free_element(res); // put off freeing, needed in render 
        free_settings(l_settings);
    }
    return 0;
}

char* lex(char* buffer, long size) {
    char* ptr = buffer;
    unsigned int i = 0;
    char* _buff = (char*)malloc(size);
    while (*ptr != '\0') {
        if (*ptr == ':') {
            _buff[i++] = *ptr++;
            while (*ptr == ' ') ptr++;
        }
        if (*ptr == '/' && *(ptr+1) == '/') {
            while(*ptr++ != '\n');
        }
        if ((*ptr == ' ' && *(ptr+1) == ' ') || (*ptr == ' ' && *(ptr+1) == ':')) {
            if(*(ptr-1) == '\n') {
                while(*ptr == ' ') ptr++;
            } else {
                while(*ptr == ' ') ptr++;
                if (*ptr == ':') continue;
                if (*ptr == ';') continue;
                ptr--;
            }
        }
        _buff[i++] = *ptr++;
    }
    _buff[i] = '\0';
    return _buff;
}


char* extract_non_special(char* buff, unsigned int *len, char* exceptions) {
    char* res = malloc(16);
    unsigned int cap = 16, size = 0;
    while (*buff++ != '\0') {
        if (size >= cap) { cap*= 2; res = realloc(res, cap);}
        if(is_special_char(*buff) && !char_in_string(*buff, exceptions) ) {
            res = realloc(res, size+1);
            res[size] = '\0';
            *len = size;
            return res;
        }
        res[size] = *buff; 
        size++;
    }
    size--;
    res = realloc(res, size+1);
    res[size] = '\0';
    *len = size;
    return res;
}

char* extract_number(char* buff, unsigned int *len) {
    char* res = malloc(5);
    unsigned int cap = 5, size = 0;
    while (*buff != '\0') {
        if (size >= cap) {cap *=2; res = realloc(res, cap);}
        if (!is_number(*buff)) {
            res = realloc(res, size+1);
            res[size] = '\0';
            *len = size;
            return res;
        }
        res[size] = *buff++;
        size++;
    }
    size--;
    res = realloc(res, size+1);
    res[size] = '\0';
    *len = size;
    return res;
}

void create_token_at_index(
        Token* tokens, unsigned int index, Token_Type type,
        char* value, unsigned int* length) {
    tokens[index].type = type;
    tokens[index].value = value;
    tokens[index].length = *length;
}

Token* tokenize(char* buffer, long* size) {
    char* ptr = buffer;
    unsigned int capacity = 10;
    unsigned int index = 0;
    Token* tokens = malloc(sizeof(Token)*capacity);
    printf("\n\n\t(+) %s\n\n", buffer);

    unsigned int *len = malloc(sizeof(unsigned int));
    while (*ptr != '\0') {
        if (index >= capacity) {
            capacity += 2; 
            Token* temp = realloc(tokens, sizeof(Token)*capacity);
            if (temp == NULL) return NULL;
            tokens = temp;
        }
        if (CHAR_AT_START('#', ptr, buffer)) {
            // id 
            create_token_at_index(tokens, index, 
                    LADDER, extract_non_special(ptr, len, "-"), len);
            ptr+=*len-1;index++;
        } 
        TOKENIZE_DYNAMIC_KEYWORD(extract_non_special(ptr, len, "-"), CHAR_AT_START('.', ptr, buffer), DOT, ptr, len, tokens, index) 
        ONE_CHAR_TOKEN('{', "{", LBRACE, ptr, len, tokens, index)
        ONE_CHAR_TOKEN('}', "}", RBRACE, ptr, len, tokens, index)
        ONE_CHAR_TOKEN(';', ";", SEMI, ptr, len, tokens, index)
        ONE_CHAR_TOKEN(':', ":", COL, ptr, len, tokens, index)
        TOKENIZE_DYNAMIC_KEYWORD(extract_non_special(ptr, len, ""), *ptr == '#', HEX_COLOR, ptr, len, tokens, index)
        TOKENIZE_DYNAMIC_KEYWORD(extract_non_special(ptr, len, "_-"), *ptr == '$', VAR_LOOKUP, ptr, len, tokens, index)
        TOKENIZE_DYNAMIC_KEYWORD(extract_number(ptr, len), is_number(*ptr), NUMBER, ptr, len, tokens, index)
        else if (*ptr == '"') {
            create_token_at_index(tokens, index,
                    STRING, extract_non_special(ptr, len, " _-+,./'!$%*(){}[]#@^&=<>"), len);
            ptr+=*len+1;index++;
        }
        TOKENIZE_KEYWORD("color", COLOR, ptr, len, tokens, index)
        TOKENIZE_KEYWORD("background-color", BACKGROUND_COLOR, ptr, len, tokens, index)
        TOKENIZE_KEYWORD("content", CONTENT, ptr, len, tokens, index)
        TOKENIZE_DYNAMIC_KEYWORD(extract_non_special(ptr+8, len, ""), live_compare("position:", ptr, len), POSITION, ptr, len, tokens, index)
        TOKENIZE_DYNAMIC_KEYWORD(extract_non_special(ptr+4, len, ""), live_compare("show:", ptr, len), SHOW, ptr, len, tokens, index)
        TOKENIZE_KEYWORD("onclick", ONCLICK, ptr, len, tokens, index)
        TOKENIZE_KEYWORD("dim", DIM, ptr, len, tokens, index)
        TOKENIZE_KEYWORD("dim", DIM, ptr, len, tokens, index)
        TOKENIZE_KEYWORD("img", IMG, ptr, len, tokens, index)
        TOKENIZE_KEYWORD("button", BUTTON, ptr, len, tokens, index)
        TOKENIZE_KEYWORD("textbox", TEXTBOX, ptr, len, tokens, index)
        TOKENIZE_KEYWORD("canvas", CANVAS, ptr, len, tokens, index)
        TOKENIZE_DYNAMIC_KEYWORD(extract_non_special(ptr, len, ""), CHAR_AT_START('!', ptr, buffer), BANG, ptr, len, tokens, index)
        ptr++;
    }
    free(len);
    index--;
    tokens = realloc(tokens, sizeof(Token)*(index+1));
    *size = index+1; 
    return tokens;
}

void print_tokens(Token* tokens, long len) {
    for(int i=0; i < len; ++i) {
#ifdef DEBUG
        printf(
                "[type=(%d, %s), value=%s, length=%u] \n", 
                tokens[i].type, lookup[tokens[i].type],
                tokens[i].value, tokens[i].length
              );  
#else
        printf(
                "[type=%d, value=%s, length=%u] \n", 
                tokens[i].type, tokens[i].value, tokens[i].length
              );
#endif
    }
}


Element* sdom(Token* tokens, long len) {
    Color zero;
    zero.r = 0; zero.g = 0; zero.b = 0, zero.a = 0;
    Element* main = create_element(NULL, NULL, 0, E_MAIN,
            4,
            create_str("main", 4),
            (Rect){(vec2){0,0},(vec2){10,10}}, 
            zero, NULL, 0);
    Element* current_parent = main, *newest_child = NULL;
    int hex;
    l_settings = get_default_settings();
    for(long i = 0; i < len; ++i) {
        switch (tokens[i].type) {
            case LADDER: // id
                newest_child = create_element(
                        current_parent, NULL, 0, E_ELEMENT,
                        tokens[i].length,
                        create_str(tokens[i].value, tokens[i].length),
                        (Rect){(vec2){0,0},(vec2){10,10}}, 
                        zero, NULL, 0);
                append_child(current_parent,
                        newest_child);
                break;
            case IMG: // img
                newest_child = create_element(
                        current_parent, NULL, 0, E_IMG,
                        tokens[i].length,
                        create_str(tokens[i].value, tokens[i].length),
                        (Rect){(vec2){0,0},(vec2){10,10}}, 
                        zero, NULL, 0);
                append_child(current_parent,
                        newest_child);
                break;
            case BUTTON: // button
                newest_child = create_element(
                        current_parent, NULL, 0, E_BUTTON,
                        tokens[i].length,
                        create_str(tokens[i].value, tokens[i].length),
                        (Rect){(vec2){0,0},(vec2){10,10}}, 
                        zero, NULL, 0);
                append_child(current_parent,
                        newest_child);
                break;
            case CANVAS: // canvas
                newest_child = create_element(
                        current_parent, NULL, 0, E_CANVAS,
                        tokens[i].length,
                        create_str(tokens[i].value, tokens[i].length),
                        (Rect){(vec2){0,0},(vec2){10,10}}, 
                        zero, NULL, 0);
                append_child(current_parent,
                        newest_child);
                break;
            case SWITCHER: // switcher
                newest_child = create_element(
                        current_parent, NULL, 0, E_SWITCHER,
                        tokens[i].length,
                        create_str(tokens[i].value, tokens[i].length),
                        (Rect){(vec2){0,0},(vec2){10,10}}, 
                        zero, NULL, 0);
                append_child(current_parent,
                        newest_child);
                break;
            case OVERLAY: // OVERLAY
                newest_child = create_element(
                        current_parent, NULL, 0, E_OVERLAY,
                        tokens[i].length,
                        create_str(tokens[i].value, tokens[i].length),
                        (Rect){(vec2){0,0},(vec2){10,10}}, 
                        zero, NULL, 0);
                append_child(current_parent,
                        newest_child);
                break;
            case POSITION:
                // code
                // PROPERTY needs to be changed so it can accomodate multiple 
                //  types of values using a union
                //  followin is how you do it for later really
                /* unsigned int j = 0, size = 0;
                while(tokens[i+1+j++].type != SEMI);
                char** values = malloc(j*sizeof(char*));
                size = j;
                while(j-- != -1) values[j] = tokens[i+1+j].value; */
                add_property(current_parent, P_POSITION, "position", &(tokens[i].value), 1);
                break;
            case SHOW:
                // code
                // PROPERTY needs to be changed so it can accomodate multiple 
                //  types of values using a union
                //  followin is how you do it for later really
                /* unsigned int j = 0, size = 0;
                while(tokens[i+1+j++].type != SEMI);
                char** values = malloc(j*sizeof(char*));
                size = j;
                while(j-- != -1) values[j] = tokens[i+1+j].value; */
                add_property(current_parent, P_SHOW, "show", &(tokens[i].value), 1);
                break;
            case ONCLICK:
                add_property(current_parent, P_ONCLICK, "onclick", &(tokens[i+2].value), 1);
                break;
            case DIM:
                //skip ":"
                current_parent->dim.pos.x = atoi(tokens[i+2].value);
                current_parent->dim.pos.y = atoi(tokens[i+3].value);
                current_parent->dim.size.x = atoi(tokens[i+4].value);
                current_parent->dim.size.y = atoi(tokens[i+5].value);
                i+=5;
            case BACKGROUND_COLOR:
                //add_property(current_parent, P_BACKGROUNDCOLOR, "background-color", tokens[i+2].value);
                hex = (int)strtol(tokens[i+2].value, NULL, 16);
                current_parent->color.r = (unsigned char)((hex >> 16) & 0xFF);
                current_parent->color.g = (unsigned char)((hex >> 8) & 0xFF);
                current_parent->color.b = (unsigned char)(hex & 0xFF);
                current_parent->color.a = (unsigned char)255;
            case COLOR:
                //add_property(current_parent, P_COLOR, "color", tokens[i+2].value);
                hex = (int)strtol(tokens[i+2].value, NULL, 16);
                current_parent->color.r = (unsigned char)((hex >> 16) & 0xFF);
                current_parent->color.g = (unsigned char)((hex >> 8) & 0xFF);
                current_parent->color.b = (unsigned char)(hex & 0xFF);
                current_parent->color.a = (unsigned char)255;
            case BANG:
                // edit "settings"
                if (str_cmp(tokens[i].value, "resolution") == 0) {
                    l_settings->resolution.x = atoi(tokens[i+1].value); // replace later
                    l_settings->resolution.y = atoi(tokens[i+2].value);
                    i+=2;
                } else if (str_cmp(tokens[i].value, "title") == 0) {
                    ASSERT(tokens[i+1].type == STRING);
                    printf("\n\n\n%s\n\n\n", tokens[i+1].value);
                    free(l_settings->title.data);
                    l_settings->title.length = tokens[i+1].length;
                    l_settings->title.data = create_str(tokens[i+1].value, tokens[i+1].length);
                }
                else if (str_cmp(tokens[i].value, "script") == 0) {
                    ASSERT(tokens[i+1].type == STRING);
                    printf("\n\n\n%s\n\n\n", tokens[i+1].value);
                    l_settings->scripts_length+=1;
                    printf("\n\n\n\t %d \n\n\n", l_settings->scripts_length);
                    if (l_settings->scripts_length > 0){
                        l_settings->scripts = realloc(l_settings->scripts, l_settings->scripts_length * sizeof(ScriptSrc));
                        l_settings->scripts[l_settings->scripts_length-1].script = create_str(tokens[i+1].value, tokens[i+1].length);
                        l_settings->scripts[l_settings->scripts_length-1].len = tokens[i+1].length;
                    }
                }
                else if (str_cmp(tokens[i].value, "method") == 0) {
                    ASSERT(tokens[i+1].type == STRING);
                    l_settings->method.length = tokens[i+1].length;
                    free(l_settings->method.data);
                    l_settings->method.data = create_str(tokens[i+1].value, tokens[i+1].length+1);
                }
                        
                // method, imports 
                // register imports
                break;
            case TEXTBOX: // textbox
                newest_child = create_element(
                        current_parent, NULL, 0, E_TEXTBOX,
                        tokens[i].length,
                        create_str(tokens[i].value, tokens[i].length),
                        (Rect){(vec2){0,0},(vec2){10,10}}, 
                        zero, NULL, 0);
                append_child(current_parent,
                        newest_child);
                break;
            case LBRACE:
                current_parent = newest_child;
                break;
            case RBRACE:
                current_parent = current_parent->_parent;
                break;
            default:
                break;
        } 
    }
    print_settings(l_settings);
    print_element(main);
    return main;
}

void write_properties(Property* props, unsigned int properties_length, FILE* fd) {
    for (unsigned int i = 0; i < properties_length; ++i) {
        fwrite(&props[i].type, sizeof(PropertyType), 1, fd);
        fwrite(&props[i].name.length, sizeof(unsigned int), 1, fd);
        if (&props[i].name.length > 0) fwrite(props[i].name.data, sizeof(char), props[i].name.length, fd);
        switch (props[i].type) {
            case P_COLOR:
                goto string;
                break;
            case P_SIZE:
                fwrite(&props[i].size, sizeof(vec2), 1, fd);
                break;
            case P_POSITION:
                fwrite(&props[i].position, 1, 1, fd);
                break;
            case P_PADDING:
                fwrite(&props[i].padding, sizeof(vec4), 1, fd);
                break;
            case P_ORDER:
                fwrite(&props[i].orientation, 1, 1, fd);
                break;
            case P_SHOW:
                fwrite(&props[i].show, sizeof(_Bool), 1, fd);
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
                fwrite(&props[i].space, sizeof(vec2), 1, fd);
                break;
        }
        continue;
    string:
        fwrite(&props[i].value.length, sizeof(unsigned int), 1, fd);
        if (props[i].value.length > 0) fwrite(props[i].value.data, sizeof(char), props[i].value.length, fd);
    }
}

void write_element(Element* res, FILE* fd) {
    // ignore _parent in write;
    fwrite(&res->type, sizeof(ElementType) + sizeof(Rect) + 4 + sizeof(unsigned int), 1, fd);
    fwrite(res->name.data, sizeof(char), res->name.length, fd);
    fwrite(&res->children_length, sizeof(unsigned int), 1, fd);
    for (unsigned int i = 0; i < res->children_length; ++i) {
        write_element(res->children[i], fd);
    }
    fwrite(&res->properties_length, sizeof(unsigned int), 1, fd);
    write_properties(res->properties, res->properties_length, fd);
}


void write_settings(Settings* settings, FILE* fd) {
    unsigned long size = 28 + settings->title.length + settings->method.length;
    for (unsigned int i = 0; i < settings->scripts_length; ++i) {
        size += 4 + settings->scripts[i].len;
    }
    fwrite(&size, sizeof(unsigned long), 1, fd);
    fwrite(&settings->resolution, sizeof(vec2) + sizeof(vec2) + sizeof(unsigned int) , 1, fd);
    fwrite(settings->title.data, sizeof(char), settings->title.length, fd);
    fwrite(&settings->method.length, sizeof(unsigned int), 1, fd);
    fwrite(settings->method.data, sizeof(char), settings->method.length, fd);
    fwrite(&settings->scripts_length, sizeof(unsigned int), 1, fd);
    for (unsigned int i = 0; i < settings->scripts_length; ++i) {
        fwrite(&settings->scripts[i].len, sizeof(unsigned int), 1, fd);
        fwrite(settings->scripts[i].script, sizeof(char), settings->scripts[i].len, fd);
        printf("\n\n\n\t %p \n\n\n", settings->scripts[i].script);
        printf("\n\n\nwriting scripts to file\n\n\n");
    }
}

void write_sdom_to_binary(Element* res, char* output_file, bool serialize) {
    printf("\t (-) %s\n", output_file);
    FILE* fd = fopen(output_file, "wb");
    if (!serialize && res != NULL) {
        fwrite(&l_settings, sizeof(Settings*), 1, fd);
        fwrite(&res, sizeof(Element*), 1, fd); 
    } else if (res != NULL) {
        write_settings(l_settings, fd);
        write_element(res, fd);
    }
    fclose(fd);
}

void free_tokens(Token* tokens, long len) {
    for(int i = len-1; i >= 0; --i) {
        if(tokens[i].type >= BANG){
            free(tokens[i].value);
        }
    }
    free(tokens);
}
