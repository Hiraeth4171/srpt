// HEADER ONLY LIBRARY
#ifndef _SDT_LIB_H
#define _SDT_LIB_H
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>

typedef struct SDTLinkedListNode SDTLinkedListNode;

typedef struct {
    size_t size;
    char* data;
} SDTString;

struct SDTLinkedListNode{
    void* value;
    SDTLinkedListNode* next;
};

typedef struct {
    void* id;
    size_t (*hash_function)(void*, size_t);
    int (*item_cmpr)(void*, void*); // item, item_id
    void (*item_free)(void*);
    size_t length;
    size_t item_size;
    SDTLinkedListNode** items;
} SDTHashtable;

/*
 * TODO:
 *  - SDTStack
 * */

static inline SDTHashtable* sdt_hashtable_init(size_t size, size_t item_size, size_t (*hash_function)(void*, size_t), int (*item_cmp)(void*, void*), void (*item_free)(void*)) {
    SDTHashtable* _hashtable = (SDTHashtable*)malloc(sizeof(SDTHashtable));
    _hashtable->items = (SDTLinkedListNode**) malloc(size * sizeof(SDTLinkedListNode*));
    for (size_t i = 0; i < size; ++i) _hashtable->items[i] = NULL;
    _hashtable->length =  size;
    _hashtable->item_size =  item_size;
    _hashtable->hash_function = hash_function;
    _hashtable->item_cmpr = item_cmp;
    _hashtable->item_free = item_free;
    return _hashtable;
}

static inline void sdt_hashtable_add(SDTHashtable* hashtable, void* item_id, void* item) {
    size_t i = hashtable->hash_function(item_id, hashtable->length);
    if (hashtable->items[i] != NULL) { // add a safegaurd
        SDTLinkedListNode* cur = hashtable->items[i];
        while (cur->next != NULL) cur = cur->next;
        cur->next = (SDTLinkedListNode*)malloc(sizeof(SDTLinkedListNode));
        *(cur->next) = (SDTLinkedListNode){.value = (void*)item, .next = NULL};
        return; // that's FUCKING insane HOW DID I EVER FUCKING FORGET THIS AM I RETARDED CHAT
    }
    hashtable->items[i] = (SDTLinkedListNode*)malloc(sizeof(SDTLinkedListNode));
    *hashtable->items[i] = (SDTLinkedListNode){.value = (void*)item, .next = NULL};
}

static inline void sdt_hashtable_remove(SDTHashtable* hashtable, void* item_id) {
    size_t i = hashtable->hash_function(item_id, hashtable->length);
    if (hashtable->items[i] != NULL) { // add a safegaurd
        SDTLinkedListNode* cur = hashtable->items[i], *prev = cur;
        while (cur != NULL) {
            if(cur->value == NULL) return; // this doesn't safegaurd much here.
            if (hashtable->item_cmpr(cur->value, item_id) == 0) {
                hashtable->item_free(cur->value);
                if(cur->next != NULL) prev->next = cur->next;
                if (prev == cur) hashtable->items[i] = NULL;
                free(cur);
                break;
            }
            prev = cur;
            cur = cur->next;
        }
    }
}

static inline void* sdt_hashtable_get(SDTHashtable* hashtable, void* item_id) {
    if (hashtable == NULL) return NULL;
    size_t i = hashtable->hash_function(item_id, hashtable->length);
    if (hashtable->items[i] != NULL) { // add a safegaurd
        SDTLinkedListNode* cur = hashtable->items[i];
        while (cur != NULL) {
            if(cur->value == NULL) return NULL; // this doesn't safegaurd much here.
            if (hashtable->item_cmpr(cur->value, item_id) == 0) return cur->value;
            cur = cur->next;
        }
    }
    return NULL;
}

static inline void sdt_hashtable_free(SDTHashtable** hashtable, void (*free_callback)(void*)) {
    if (hashtable == NULL || *hashtable == NULL) return;
    if ((*hashtable)->items == NULL) goto free_ptr;
    for (size_t i = 0; i < (*hashtable)->length; ++i) {
        if ((*hashtable)->items[i] == NULL) continue;
        SDTLinkedListNode* cur = (*hashtable)->items[i];
        while (cur != NULL) {
            free_callback(cur->value);
            SDTLinkedListNode* tmp = cur;
            cur = cur->next;
            free(tmp);
        }
    }
    free((*hashtable)->items);
free_ptr:
    free(*hashtable);
}

static inline void sdt_hashtable_print(SDTHashtable* hashtable, void (*print_callback)(void*)) {
    for (size_t i = 0; i < hashtable->length; ++i) {
        SDTLinkedListNode* cur = hashtable->items[i];
#if defined(_WIN32) || defined(_WIN64)
        printf("%llu : ", i);
#elif defined(__linux__)
        printf("%lu : ", i);
#endif
        while (cur != NULL && cur->value != NULL) {
            print_callback(cur->value);
            printf(" -> ");
            cur = cur->next;
        }
        printf("END\n");
    }
}

static inline char* sdt_read_file(const char* filepath, long* length) {
    // access first
    FILE* fd = fopen(filepath, "r");
    fseek(fd, 0L, SEEK_END);
    long pos = ftell(fd);
    *length = pos;
    rewind(fd);
    char* _buff = (char*)malloc(pos+1);
    fread(_buff, 1, pos, fd);
    _buff[pos] = '\0';
    fclose(fd);
    return _buff;
}

/*
 *  Function provided by Octe™
 */
static inline char* sdt_match_until_but_better(char* src, const char ch, size_t *len) {
    if (!*src) return NULL;
    char* ptr = src;
    for (;*ptr != ch && *ptr != '\0'; ++ptr);
    char* str = (char*)calloc((*len = ptr - src)+1, sizeof(char));
    if (!str) return NULL;
    for (size_t i = 0; i < *len; ++i) *str++ = *src++;
    return str;
}

static inline bool sdt_check_opts(char* ptr, char* chs) {
    for (char* ch = chs; *ch != '\0'; ++ch) if (*ptr == *ch) return false;
    return true;
}

/*
 *  Function inspired by Octe™
 */
static inline char* sdt_match_until_opts_but_better(char* src, const char* chs, size_t *len) {
    if (!*src) return NULL;
    char* ptr = src; bool flag = true;
    for (const char* ch = chs; *ch != '\0'; ++ch) if (*ptr == *ch) flag = false;
    for (;!flag && *ptr != '\0'; ++ptr);
    char* str = (char*)calloc((*len = ptr - src)+1, sizeof(char));
    if (!str) return NULL;
    for (size_t i = 0; i < *len; ++i) *str++ = *src++;
    return str;
}

static inline bool sdt_check_match(char* src, const char* match, size_t len) {
    size_t i = 0;
    char *ptr = src;
    for(;*ptr == match[i] && match[i] != '\0'; ++ptr, ++i);
    return (i-len);
}

static inline char* crt_str(char* str, size_t size) {
    char* _buff = (char*)malloc(size), *src = str, *dst = _buff;
    while (*src) *dst++ = *src++;
    *dst = '\0';
    return _buff;
}

#endif
