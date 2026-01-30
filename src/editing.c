#include "utilities.h"

int getPosInBytes(int at, char * input , int len ){
    int i = 0;
    int pos = 0;
    char c ;
    while(i<len && pos != at){
        c = input[i];
        i+=utf8_len(c);
        pos++;
    }
    return i;
}
int getPos(int bytes, char *input){
    int at = 0;
    int i = 0;
    char c ;
    while ( i < bytes ){
        c = input[i];
        i += utf8_len(c);
        at++;
    }
    return at;
}
void insertCharInRow(struct string *row, int at ,char* input , int inputLength){
        row->b = realloc(row->b , row->lenByte + inputLength + 1);
        memmove(row->b+at+inputLength ,row->b+at ,row->lenByte-at+1);
        memcpy(row->b+at , input , inputLength);
        row->lenByte += inputLength;
        row->len += getPos(inputLength, input);
}
void insertChar(char* input , int inputLength){
    if (e.cy+e.rowoff >= e.rowsNum ){ 
        e.rowBuff = (struct string*)realloc(e.rowBuff , sizeof(struct string) * (e.cy+e.rowoff + 1));
        for (int i = e.rowsNum ; i<e.cy+e.rowoff+1 ;i++)
            initString(&e.rowBuff[i]);
        e.rowsNum = e.cy+e.rowoff+1;
    }
    
    struct string* current_row = e.rowBuff+e.cy+e.rowoff;
    int current_col = e.cx+e.coloff;
    int pos = getPosInBytes(current_col, current_row->b , current_row->lenByte);
    
    if(current_col < current_row ->len){
        insertCharInRow(current_row, pos, input , inputLength);
    }
    else{
        char buf[1000];
        int i = 0;
        while (i <  current_col-current_row->len) buf[i++]= ' ';
        memcpy(buf+i , input , inputLength);
        stringAppend(current_row,buf,i+inputLength);
    }

    e.modification_num+=inputLength;
}
void removeCharInRow(struct string* row , int at , int lenByte){
        if (lenByte > row->lenByte)
            return ;
        memcpy(row->b+at-lenByte ,row->b+at ,row->lenByte - at);
        row->lenByte -= lenByte;
        row->len -= getPos(lenByte,row->b+at-lenByte);
        row->b[row->lenByte] = '\0';
}

int removeChar(){

    if (e.cy+e.rowoff >= e.rowsNum)
        return 0;
    int current_col = e.cx + e.coloff;
    struct string* current_row = e.rowBuff+e.cy+e.rowoff;
    int pos = getPosInBytes(current_col , current_row->b , current_row->lenByte);

    if ( current_col >0  && pos <= current_row->lenByte ){
    
        removeCharInRow(current_row, pos , 
                utf8_len(current_row->b[getPosInBytes(current_col-1,current_row->b , current_row->lenByte)])
                );

        e.modification_num++;
        return 1;
    }
    else if (current_col == 0 ){

        if (e.cy+e.rowoff){
            if(current_row->lenByte != 0) stringAppend(current_row-1 ,current_row->b, current_row->lenByte);
            if(e.cy+e.rowoff < e.rowsNum-1) {
                free(current_row->b);
                memmove(current_row,
                        current_row+1,
                        sizeof(struct string)*(e.rowsNum - (e.cy+e.rowoff+1) )
                );
            }
        }
        else if (e.rowsNum > 1 || current_row->lenByte) return 0;

        e.rowsNum--;
        e.modification_num++;
        return 1;
    }

    return 0;
}

void insertNewLine(){
    int current_row = e.rowoff+e.cy;
    int current_col = e.coloff+e.cx;

    // treat current_row out of file file length 
    if (current_row >= e.rowsNum ){ 
        e.rowBuff = (struct string*)realloc(e.rowBuff , sizeof(struct string) * (current_row + 2));
        for (int i = e.rowsNum ; i<current_row+2 ;i++)
            initString(&e.rowBuff[i]);
        e.rowsNum = current_row+2;

        e.modification_num++;
        return;
    }

    int pos = getPosInBytes(current_col , (e.rowBuff+current_row)->b , (e.rowBuff+current_row)->lenByte );
    
    // Allocate memory to a new line
    e.rowBuff = (struct string*)realloc(e.rowBuff , sizeof(struct string) * (e.rowsNum+1) );
    memmove(e.rowBuff+current_row+2 ,
            e.rowBuff+current_row+1 ,
            sizeof(struct string) * (e.rowsNum-1-current_row)
    );
    e.rowsNum++;
    
    // add tail to next new line
    int nextLineLen = (e.rowBuff[current_row].lenByte-pos >= 0 ) ? (e.rowBuff[current_row].lenByte-pos) : 0 ; 
    e.rowBuff[current_row+1].b = (char*)malloc( sizeof(char) * (nextLineLen+1) );
    if(nextLineLen){ 
        memcpy(e.rowBuff[current_row+1].b ,e.rowBuff[current_row].b+pos ,nextLineLen);
        e.rowBuff[current_row+1].b[nextLineLen] = '\0';
    }
    e.rowBuff[current_row+1].lenByte = nextLineLen;
    e.rowBuff[current_row+1].len = getPos(nextLineLen , e.rowBuff[current_row+1].b );
    
    // rewrite current line
    int currentLineLen = e.rowBuff[current_row].lenByte - nextLineLen; 
    char * temp = e.rowBuff[current_row].b;
    e.rowBuff[current_row].b = (char*)realloc(e.rowBuff[current_row].b , sizeof(char) * (currentLineLen+1));
    if(currentLineLen) memcpy(e.rowBuff[current_row].b ,temp ,currentLineLen);
    e.rowBuff[current_row].lenByte = currentLineLen;
    e.rowBuff[current_row].len = getPos(currentLineLen , e.rowBuff[current_row].b );
    if(currentLineLen) e.rowBuff[current_row].b[currentLineLen] = '\0';

    e.modification_num++;
}

void saveToDisk (){
    FILE* file = fopen(e.filePath,"w+");
    if (file == NULL) die("fopen") ;
    
    for (int i =0 ;i<e.rowsNum ;i++){
        if(e.rowBuff[i].lenByte) write(fileno(file), e.rowBuff[i].b,e.rowBuff[i].lenByte);

        if (i != e.rowsNum-1){
            write(fileno(file),"\n",1);
        }
    }
    e.modification_num = 0;
    fclose(file);
}
