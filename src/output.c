#include "utilities.h"

// Helper method


void refreshScreen(){
    if (getWindowSize(&e.windowsLength, &e.windowsWidth) == -1)
        die("getWindowSize");
    e.windowsLength -=2;
    

    struct string ab;
    initString(&ab);

    stringAppend(&ab, "\x1b[2J\x1b[3J",8);
    stringAppend(&ab, "\x1b[H" ,3);
    
    drawRows(&ab);
    drawStatusLine(&ab);

    time_t current_time;
    time(&current_time);
    if ((int)current_time - e.messageTime >= e.messageWait ) 
        freeMessage();

    if (e.message.len) drawMessage(&ab,e.message);
    if (e.message.len > e.windowsWidth) e.windowsLength--;

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
                if (welcomelen > e.windowsWidth) welcomelen = e.windowsWidth;
                int padding = (e.windowsWidth-welcomelen)/2;
                stringAppend(ab, "⮚" , 4);
                if (padding)padding --;
                while(padding){
                    stringAppend(ab, " " , 1);
                    padding--;
                }
                
                stringAppend(ab, welcome, welcomelen);
                
            } 
            else {
              stringAppend(ab, "⮚", 4);
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
    if (destination->b == NULL) destination->b = malloc(len+1);
    else destination->b = realloc (destination->b,len+1);
    memcpy(destination->b ,message , len+1);
    destination->len = len;
   
    time_t current_time;
    time(&current_time);
    e.messageTime = (int)current_time ;
}
void freeMessage(){
    free(e.message.b);
    e.message.b = NULL;
    e.message.len = 0;
}
