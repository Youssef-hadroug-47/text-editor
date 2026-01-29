#include "utilities.h"


// Helper method
void moveCursorToIndex(int idx){
    if (idx > e.cx+e.coloff){
        if (idx -(e.cx+e.coloff) < e.windowsWidth - e.cx){
            e.cx +=idx - (e.cx+e.coloff);
        }
        else {
            e.cx = e.windowsWidth-1;
            e.coloff = idx -e.windowsWidth+1 ;
        }
    }
    else {
        if ((e.cx+e.coloff)-idx < e.cx){
            e.cx = idx - e.coloff;
        }
        else {
            e.coloff = idx ; 
            e.cx = 0;
        }

    }
}
void upArrow(){
    int row = e.cy+e.rowoff;
    if(row > 0 && row -1 < e.rowsNum &&
            ( e.cx+e.coloff >= e.rowBuff[row].len  ||  e.cx+e.coloff > e.rowBuff[row-1].len ) ) 
        moveCursorToIndex(e.rowBuff[row-1].len); 
    if (e.cy) e.cy--; 
    else if(e.rowoff) e.rowoff--;
}
void downArrow(){
    int row = e.cy+e.rowoff;
    if (row == e.rowsNum-1 )
        moveCursorToIndex(0);

    if(row+1 < e.rowsNum  && 
            ( e.cx+e.coloff >= e.rowBuff[row].len || e.cx+e.coloff > e.rowBuff[row+1].len)) 
        moveCursorToIndex(e.rowBuff[row+1].len);

    if(e.cy != e.windowsLength -1)  e.cy++;
    else e.rowoff++;
}
void rightArrow(){
    if(e.cy+e.rowoff<e.rowsNum  && e.cx + e.coloff== e.rowBuff[e.cy+e.rowoff].len) {
        //if (e.cc != e.windowsLength -1) e.cy++;
        //else e.rowoff++;
        //e.cx=0;
        //e.coloff=0;
    }
    else
    if(e.cx != e.windowsWidth -1) e.cx++;
    else e.coloff++;
}
void leftArrow(){
    if (e.cx != 0) e.cx--;
    else {
        if (e.coloff) e.coloff--;
        else if(e.cy+e.rowoff < e.rowsNum && e.cy+e.rowoff != 0){
                //if (e.cy) e.cy--;
                //else e.rowoff--;
                //dollarSign(); 
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
void character(char* input , int inputLength){
    if ( (inputLength == 1 && !iscntrl(input[0]) ) || inputLength > 1 ){
        insertChar(input ,inputLength);
        if (e.cx != e.windowsWidth-1) e.cx++;
        else e.coloff++;
    }
}
void tab(){
    for (int i =0 ;i<4;i++){
        char c = ' ';
        insertChar(&c,1);
        if (e.cx != e.windowsWidth-1) e.cx++;
        else e.coloff++;
    }
}
void dollarSign(){
    if (e.rowBuff == NULL) return;
    int len= e.rowBuff[e.cy+e.rowoff].len ;
    if(len > e.windowsWidth) {
        e.coloff = len - e.windowsLength;
    }
    e.cx= len ;
}



// Help method
int nextSpace(char* string , int at){
    int len = strlen(string);
    if (at >= len -1) return -1;
    at++;
    int i = at;
    while(i < len && string[i] == ' '){i++;}
    if (i == len) return -1;
    while(i < len && string[i] != ' '){i++;}
    return --i;
}
void gotoNextWord(){
    if (e.rowBuff == NULL) return; 
    int at = e.cx+e.coloff;
    int nextWord = nextSpace(e.rowBuff[e.cy+e.rowoff].b, at);
    if (nextWord == -1){
        if(e.cy < e.windowsLength-1 && e.cy < e.rowsNum-1) e.cy++;
        else if(e.cy+e.rowoff < e.rowsNum -1 ) e.rowoff++;
        else return;

        e.coloff=0;
        
        int next = nextSpace(e.rowBuff[e.cy+e.rowoff].b, 0);
        if (next == -1)
            moveCursorToIndex(0);
        moveCursorToIndex(next);
    }
    else
        moveCursorToIndex(nextWord);
 
}
// Help method
int prevSpace(char* string , int at){
    int len = strlen(string);

    if (at == 0) return -1;
    at--;

    int i = at;
    while( i >= 0 && string[i] == ' '){i--;}
    if (i == -1) return -1;
    while(i >= 0 && string[i] != ' '){i--;}
    if (i == -1) return 0;
    else return ++i;
    

    return 0;
}
void gotoPrevWord(){
    if (e.rowBuff == NULL) return;
    int prevWord = prevSpace(e.rowBuff[e.cy+e.rowoff].b , e.cx+e.coloff);
    if (prevWord == -1){
        if (e.cy) e.cy--;
        else if(e.rowoff) e.rowoff--;
        else return;

        int prev = prevSpace(e.rowBuff[e.cy+e.rowoff].b, e.rowBuff[e.cy+e.rowoff].len);
        if (prev == -1) e.cx = 0;
        else moveCursorToIndex(prev);
    }
    else
        moveCursorToIndex(prevWord);

}
int moveLine(int x){
    int currentY = e.cy+e.rowoff;
    if (x != -1 && x != 1) return -1;
    if (x == -1 && currentY == 0) return -1;
    if (x == 1 && currentY == e.rowsNum-1) return -1 ;
    struct string temp = e.rowBuff[currentY];

    memcpy(e.rowBuff+currentY , e.rowBuff+currentY+x , sizeof(struct string));
    memcpy(e.rowBuff+currentY+x , &temp , sizeof(struct string));
    return 1;
}
void moveLineDown(){
    if (moveLine(1) == 1){
        if (e.cy != e.windowsLength-1) e.cy++;
        else e.rowoff ++;
    }

}
void moveLineUp(){
    if(moveLine(-1) == 1){
        if (e.cy) e.cy--;
        else e.rowoff --;
    }
}
