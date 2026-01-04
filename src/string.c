#include "utilities.h"

void initString(struct string *ab){
    ab->b = (char*)malloc(sizeof(char));
    ab->len = 0;
    ab->b[0]='\0';
}
void stringAppend(struct string *ab , const char* c , size_t len){
    ab->b = realloc(ab->b, ab->len + len + 1);

    memcpy(ab->b + ab->len , c, len);
    ab->len +=len;
    ab->b[ab->len] = '\0';
}
void stringFree(struct string* ab){
    free(ab->b);
}
