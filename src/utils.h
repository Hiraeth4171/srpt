
#define bool _Bool
#define TRUE 1
#define FALSE 0

char* read_file(const char* filepath, long* length);
bool live_compare(char* keyword, char* ptr, unsigned int *len);
bool char_in_string (char c, char* str);
bool is_number(char c);
bool is_special_char(char c); 
int str_cmp(const char* s1, const char* s2);
void str_cpy(const char* src, char* dest);
unsigned int str_len(const char* s);
char* create_str(char* str, unsigned int length);
