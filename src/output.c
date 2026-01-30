#include "utilities.h"


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
        stringFree(&e.message);

    if (e.message.len) drawMessage(&ab,e.message);
    if (e.message.len > e.windowsWidth) e.windowsLength--;

    char ch[100];
    int len = snprintf(ch,sizeof(ch), "\x1b[%d;%dH",(e.cy)+1 ,(e.cx)+1+e.startingX);
    stringAppend(&ab, ch ,len);
    
    write(STDOUT_FILENO ,ab.b,ab.lenByte);
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
            e.modification_num > 0 ? "(modified)" : "" 
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
        //char padding[10];
        //int len = snprintf(padding, sizeof(padding),"%d", i);
        
        //int paddingLen = e.startingX - len;
        //snprintf(number, sizeof(number), "\e[38;5;23m%02d\e[0m",i);
        //stringAppend(ab,number,strlen(number));
        if (i>=e.rowsNum){
            if (e.rowBuff == NULL ) {
                    char* editorName [] ={ 
                            "rrrrr   rrrrrrrrr   aaaaaaaaaaaaawwwwwww           wwwww           wwwwwww eeeeeeeeeeee",    
                            "r::::rrr:::::::::r  a::::::::::::aw:::::w         w:::::w         w:::::wee::::::::::::ee ", 
                            "r:::::::::::::::::r aaaaaaaaa:::::aw:::::w       w:::::::w       w:::::we::::::eeeee:::::ee",
                            "rr::::::rrrrr::::::r         a::::a w:::::w     w:::::::::w     w:::::we::::::e     e:::::e",
                            " r:::::r     r:::::r  aaaaaaa:::::a  w:::::w   w:::::w:::::w   w:::::w e:::::::eeeee::::::e",
                            " r:::::r     rrrrrrraa::::::::::::a   w:::::w w:::::w w:::::w w:::::w  e:::::::::::::::::e ",
                            " r:::::r           a::::aaaa::::::a    w:::::w:::::w   w:::::w:::::w   e::::::eeeeeeeeeee  ",
                            " r:::::r          a::::a    a:::::a     w:::::::::w     w:::::::::w    e:::::::e           ",
                            " r:::::r          a::::a    a:::::a      w:::::::w       w:::::::w     e::::::::e          ",
                            " r:::::r          a:::::aaaa::::::a       w:::::w         w:::::w       e::::::::eeeeeeee  ",
                            " r:::::r           a::::::::::aa:::a       w:::w           w:::w         ee:::::::::::::e  ",
                            " rrrrrrr            aaaaaaaaaa  aaaa        www             www            eeeeeeeeeeeeee  "
                        };
                    int nameHeight = 12;
                    int nameWidth = strlen(editorName[2]);
                    int startIdx = (e.windowsLength - nameHeight)/2 > 0 ? (e.windowsLength - nameHeight)/2 : 0;

                    if ( i >= startIdx + nameHeight || i < startIdx){
                        stringAppend(ab,"🟆",4);
                    }
                    else if (i >= startIdx ){
                        int x = i-startIdx; 
                        
                        int horizontalPadding = (e.windowsWidth-1-nameWidth)/2 > 0 ? (e.windowsWidth-1-nameWidth)/2 : 0; 
                        int idx = (horizontalPadding - e.coloff) > 0 ? horizontalPadding - e.coloff : 0;
                        stringAppend(ab, "🟆", 4);
                        while ( idx > 0){
                            stringAppend(ab, " ", 1);
                            idx--;
                        }

                        int lineStart = e.coloff - horizontalPadding > 0 ? e.coloff - horizontalPadding: 0;
                        if (lineStart < strlen(editorName[x])){ 
                            int len = strlen(editorName[x]) - lineStart >= e.windowsWidth-1 ? e.windowsWidth-1 : strlen(editorName[x]) - lineStart; 
                            stringAppend(ab, &editorName[x][lineStart], len);
                        }
                    }
            }
            else {
              stringAppend(ab, "🟆", 4);
            }
        }
        else {
            int len = (e.rowBuff+i)->lenByte - getPos(
                    e.coloff , (e.rowBuff+i)->b );
            if (len < 0) len =0;
            if (e.rowBuff[i].len - e.coloff > e.windowsWidth) len = getPos( e.windowsWidth , (e.rowBuff+i)->b );
            stringAppend(ab, (e.rowBuff+i)->b+e.coloff  , len);
        }
        stringAppend(ab ,"\r\n" ,2);
    }
}
void drawMessage(struct string *ab, struct string message){
    const char* color = "\e[38;5;214m";
    const char* reset = "\e[0m";
    stringAppend(ab, color, strlen(color));
    stringAppend(ab, message.b, message.lenByte);
    stringAppend(ab, reset, strlen(reset));
}
void writeMessage(struct string *destination , char* message , int len){
    if (destination->b == NULL) destination->b = malloc(len+1);
    else destination->b = realloc (destination->b,len+1);
    memcpy(destination->b ,message , len+1);
    destination->lenByte = len;
    destination->len = getPos(len , message );
   
    time_t current_time;
    time(&current_time);
    e.messageTime = (int)current_time ;
}
