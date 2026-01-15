#include "utilities.h"

void upArrow(){
    if(e.cy+e.rowoff -1 < e.rowsNum  && e.cy+e.rowoff != 0 && e.cx >= e.rowBuff[e.cy+e.rowoff].len-1) 
        e.cx =  e.rowBuff[e.cy-1+e.rowoff].len-1 >= 0 ? 
                e.rowBuff[e.cy-1+e.rowoff].len-1 : 
                0 ;
    else e.cx = 0;
    if (e.cy) e.cy--; 
    else if(e.rowoff) e.rowoff--;
}
void downArrow(){
    if(e.cy+e.rowoff+1 < e.rowsNum  && e.cx >= e.rowBuff[e.cy+e.rowoff].len-1){ 
        e.cx  = (e.rowBuff[e.cy+1+e.rowoff].len > 0) ? 
                e.rowBuff[e.cy+1+e.rowoff].len-1 :
                0 ;
    }
    else e.cx = 0;
    if(e.cy != e.windowsLength -1)  e.cy++;
    else e.rowoff++;
}
void rightArrow(){
    if(e.cy+e.rowoff<e.rowsNum  && e.cx + e.coloff== e.rowBuff[e.cy+e.rowoff].len) {
        if (e.cy != e.windowsLength -1) e.cy++;
        else e.rowoff++;
        e.cx=0;
        e.coloff=0;
    }
    else if(e.cx != e.windowsWidth -1) e.cx++;
    else e.coloff++;
}
void leftArrow(){
    if (e.cx != 0) e.cx--;
    else {
        if (e.coloff) e.coloff--;
        else if(e.cy+e.rowoff < e.rowsNum && e.cy+e.rowoff != 0){
                if (e.cy) e.cy--;
                else e.rowoff--;
                dollarSign(); 
        }
    }
}
void backspace(){
    int prev_len;
    prev_len = (e.cy+e.rowoff == 0) ? 0 : e.rowBuff[e.cy+e.rowoff-1].len; 
    
    if (!removeChar()) return;
    
    if(e.cx)e.cx--;
    else if(e.coloff) e.coloff--;
    else {
        e.cx = prev_len ; 
        if(e.cy) e.cy--;
        else if (e.rowoff) e.rowoff--;
    }
}
void enter(){
    insertNewLine();
    if (e.cy != e.windowsLength-1) e.cy++;
    else e.rowoff++;
    e.cx=0;
    e.coloff=0;
}
void character(char c){
    if (!iscntrl(c)){
        insertChar(c);
        if (e.cx != e.windowsWidth-1) e.cx++;
        else e.coloff++;
    }
}
void dollarSign(){
    int len= e.rowBuff[e.cy+e.rowoff].len ;
    if(len > e.windowsWidth) {
        e.coloff = len - e.windowsLength;
    }
    e.cx= len-1 -e.coloff;
}
