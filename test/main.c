#include <stdio.h>
int utf8_len(unsigned char c) {
    if ((c & 0x80) == 0) return 1;
    if ((c & 0xE0) == 0xC0) return 2;
    if ((c & 0xF0) == 0xE0) return 3;
    if ((c & 0xF8) == 0xF0) return 4;
    return -1;
}
int main(){
    char code[4];              
    code[0] = -16;
    code[1] = -110;
    code[2] = -128;
    code[3] = -128;
    printf("%d %d %d %d %d %s",utf8_len(code[0]),code[0],code[1],code[2],code[3],code);
    
}
