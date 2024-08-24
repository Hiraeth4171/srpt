#include <stdio.h>
#include <stdlib.h>
#include "./utils.h"

char* read_file(const char* filepath, long* length) {
    FILE* fd = fopen(filepath, "r");
    fseek(fd, 0L, SEEK_END);
    long pos = ftell(fd);
    *length = pos; 
    rewind(fd);
    char* _buff = malloc(sizeof(char)*pos+1);
    fread(_buff, 1, pos, fd);
    fclose(fd);
    _buff[*length] = '\0';
    return _buff;
}

bool is_special_char(char c) {
    return !((c <= 90 && c >= 65) ||
             (c <=  122 && c >= 97) ||
             (c <= 57 && c >= 48));
}

bool is_number(char c) {
    return ((c <= 57 && c >= 48) || c == '.' || c == '%');
}

bool char_in_string (char c, char* str) {
    char* ptr = str;
    if (*ptr == '\0') return FALSE;
    while(*ptr != '\0') {
        if (*ptr++ == c) return TRUE;
    }
    return FALSE;
}

bool live_compare(char* keyword, char* ptr, unsigned int *len) {
    if (*ptr != keyword[0]) return FALSE;
    char* k_ptr = keyword; char* b_ptr = ptr;
    unsigned int i = 0;
    while (*k_ptr != '\0') {
        if (*k_ptr != *b_ptr) return FALSE;
        k_ptr++;b_ptr++;i++;
    }
    *len = i;
    return TRUE;
}

int str_cmp(const char* s1, const char* s2) {
    while (*s1 && *s1 == *s2) {s1++; s2++;}
    return *(const unsigned char*)s1 - *(const unsigned char*)s2;
}

void str_cpy(const char* src, char* dest) {
    unsigned int len = 0;
    while(*src != '\0') {
        *dest++ = *src++; len++;
    }
    dest[len-1] = '\0';
}

unsigned int str_len(const char* s){
    unsigned int i = 0;
    while(*s++ != '\0') i++;
    return i;
}

char* create_str(char* str, unsigned int length) {
    char* str2 = calloc(1, length+1);
    char* ptr = str2;
    while (*str != '\0') *ptr++ = *str++;
    str2[length] = '\0';
    return str2;
}
