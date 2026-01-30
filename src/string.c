#include "utilities.h"

void initString(struct string *ab){
    ab->b = (char*)malloc(sizeof(char));
    ab->lenByte = 0;
    ab->len = 0;
    ab->b[0]='\0';
}
void stringAppend(struct string *ab , const char* c , int lenByte){
    ab->b = realloc(ab->b, ab->lenByte + lenByte + 1);
    int pos = getPos(lenByte , (char*)c );
    memcpy(ab->b + ab->lenByte , c, lenByte);
    ab->lenByte += lenByte;
    ab->len += pos;
    ab->b[ab->lenByte] = '\0';
}
void stringFree(struct string* ab){
    free(ab->b);
    ab->b = NULL;
    ab->len = 0;
    ab->lenByte = 0;
}
