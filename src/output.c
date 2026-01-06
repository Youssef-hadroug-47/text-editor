#include "utilities.h"
#include <stdio.h>


void refreshScreen(){
    struct string ab;
    initString(&ab);


    stringAppend(&ab,"\x1b[2J",4);
    stringAppend(&ab, "\x1b[H" ,3);
    
    drawRows(&ab);
    
    char ch[100];
    snprintf(ch,sizeof(ch), "\x1b[%d;%dH",(e.cy)+1 ,(e.cx)+1);
    stringAppend(&ab, ch ,strlen(ch));
    
    write(STDOUT_FILENO ,ab.b,ab.len);
    stringFree(&ab);
}

void drawRows(struct string *ab){
    for (int y =0;y<e.windowsLength;y++){
        int i= y+e.rowoff;
        //char number[32] ;
        //snprintf(number, sizeof(number), "\e[38;5;238m%02d│ \e[0m",i);
        //stringAppend(ab,number,strlen(number));
        if (i>=e.rowsNum){
            if (i== 0) {
                char welcome[80];
                
                int welcomelen = snprintf(welcome, sizeof(welcome),
                        "Welcome to my text editor !");
               if (welcomelen > e.windowsLength) welcomelen = e.windowsLength;
                
                stringAppend(ab, welcome, welcomelen);
                
            } 
            else {
              stringAppend(ab, "~", 1);
            }
        }
        else {
            int len = (e.rowBuff+i)->len - e.coloff;
            if (len < 0) len =0;
            if (len > e.windowsWidth) len = e.windowsWidth;
            stringAppend(ab, (e.rowBuff+i)->b+e.coloff  , len);
        }
        if (y<e.windowsLength -1)
            stringAppend(ab ,"\r\n" ,2);
    }
}
