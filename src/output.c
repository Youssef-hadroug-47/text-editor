#include "utilities.h"


void refreshScreen(){
    struct string ab;
    initString(&ab);


    stringAppend(&ab,"\x1b[2J",4);
    stringAppend(&ab, "\x1b[H" ,3);
    
    drawRows(&ab);
    drawStatusLine(&ab);

    time_t current_time;
    time(&current_time);
    if ((int)current_time - e.messageTime >= e.messageWait ) {
        stringFree(&e.message);
        initString(&e.message);
    }
    if (e.message.len) drawMessage(&ab,e.message);

    char ch[100];
    snprintf(ch,sizeof(ch), "\x1b[%d;%dH",(e.cy)+1 ,(e.cx)+1);
    stringAppend(&ab, ch ,strlen(ch));
    
    write(STDOUT_FILENO ,ab.b,ab.len);
    stringFree(&ab);
}

void drawStatusLine(struct string *ab){
    const char* color =  "\e[48;5;25m";
    const char* reset = "\e[0m";
    
    char statusLine[100];
    int len = snprintf(statusLine,sizeof(statusLine),
            (e.rowsNum == 0 || e.rowsNum == 1) ? "%.20s - %d line %s" : "%.20s - %d lines %s",
            e.filename ? e.filename : "[No Name]",
            e.rowsNum ,
            e.modification_num ? "(modified)" : "" 
            );
    char percent[30];
    int len_percent;
    
    if (e.cy+e.rowoff == 0 )
        len_percent = snprintf(percent , sizeof(percent),"TOP");
    else if (e.cy + e.rowoff >= e.rowsNum -1)
        len_percent = snprintf(percent , sizeof(percent),"BOT");
    else
        len_percent = snprintf(percent , sizeof(percent),"%d%%",(e.cy+e.rowoff)*100/e.rowsNum);

    char rstatus[80];
    int rlen = snprintf(rstatus, sizeof(rstatus),"%s  %d:%d",percent,e.cy+1,e.cx+1);
    
    stringAppend(ab,color,strlen(color));

    stringAppend(ab,statusLine,len);
    for (int i = len ;i<e.windowsWidth-rlen; i++)
        stringAppend(ab," ",1);
    if (e.windowsWidth - rlen > len) stringAppend(ab,rstatus,rlen);
                                                                                                                                   
    stringAppend(ab,reset,strlen(reset));
    stringAppend(ab,"\n\r",2);
}

void drawRows(struct string *ab){
    for (int y =0;y<e.windowsLength;y++){
        int i= y+e.rowoff;
        //char number[32] ;
        //snprintf(number, sizeof(number), "\e[38;5;238m%02d│ \e[0m",i);
        //stringAppend(ab,number,strlen(number));
        if (i>=e.rowsNum){
            if (e.rowBuff == NULL && i == e.windowsLength/3 ) {
                char welcome[80];
                 
                int welcomelen = snprintf(welcome, sizeof(welcome),
                        "Welcome to my text editor !");
                if (welcomelen > e.windowsLength) welcomelen = e.windowsLength;
                int padding = (e.windowsWidth-welcomelen)/2;
                stringAppend(ab, "~" , 1);
                if (padding)padding --;
                while(padding){
                    stringAppend(ab, " " , 1);
                    padding--;
                }
                
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
        stringAppend(ab ,"\r\n" ,2);
    }
}
void drawMessage(struct string *ab, struct string message){
    const char* color = "\e[38;5;214m";
    const char* reset = "\e[0m";
    stringAppend(ab, color, strlen(color));
    stringAppend(ab, message.b, message.len);
    stringAppend(ab, reset, strlen(reset));
}
void writeMessage(struct string *destination , char* message , int len){
    stringFree(&e.message);
    initString(&e.message);
    stringAppend(&e.message ,message ,len);
    time_t current_time;
    time(&current_time);
    e.messageTime = (int)current_time ;
}
