#include "utilities.h"

void insertCharInRow(struct string *row, int at ,char c){
        row->b = realloc(row->b , row->len + 2);
        memmove(row->b+at+1 ,row->b+at ,row->len-at+1);
        row->b[at]=c;
        row->len++;
}
void insertChar(char c){
    if (e.cy+e.rowoff >= e.rowsNum ){ 
        e.rowBuff = (struct string*)realloc(e.rowBuff , sizeof(struct string) * (e.cy+e.rowoff + 1));
        for (int i = e.rowsNum ; i<e.cy+e.rowoff+1 ;i++)
            initString(&e.rowBuff[i]);
        e.rowsNum = e.cy+e.rowoff+1;
    }

    struct string* current_row = e.rowBuff+e.cy+e.rowoff;
    int current_col = e.cx+e.coloff;
    
    if(current_col < current_row ->len){
        insertCharInRow(current_row, current_col, c);
    }
    else{
        char buf[1000];
        int i = 0;
        while (i <  current_col-current_row->len) buf[i++]= ' ';
        buf[i] = c;
        stringAppend(current_row,buf,i+1);
    }

    e.modification_num++;
}
void removeCharInRow(struct string* row , int at){
        memcpy(row->b+at-1 ,row->b+at ,row->len - at);
        row->len --;
        row->b[row->len] = '\0';
}

int removeChar(){
    int current_col = e.cx + e.coloff;
    struct string* current_row = e.rowBuff+e.cy+e.rowoff;

    if ( current_col >0 && e.cy+e.rowoff < e.rowsNum ){
        
        removeCharInRow(current_row, current_col);

        e.modification_num++;
        return 1;
    }
    else if (current_col == 0 && e.cy+e.rowoff < e.rowsNum ){

        if (e.cy+e.rowoff != 0){
            if(current_row->len != 0) stringAppend(current_row-1 ,current_row->b, current_row->len);

            if(e.cy+e.rowoff < e.rowsNum-1) {
                memmove(current_row,
                        current_row+1,
                        sizeof(struct string)*(e.rowsNum - (e.cy+e.rowoff+1) )
                );
            }
        }
        
        free(e.rowBuff[e.rowsNum-1].b);
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
    
    // Allocate memory to a new line
    e.rowBuff = (struct string*)realloc(e.rowBuff , sizeof(struct string) * (e.rowsNum+1) );
    memmove(e.rowBuff+current_row+2 ,
            e.rowBuff+current_row+1 ,
            sizeof(struct string) * (e.rowsNum-1-current_row)
    );
    e.rowsNum++;
    
    // add tail to next new line
    int nextLineLen = (e.rowBuff[current_row].len-current_col >= 0 ) ? (e.rowBuff[current_row].len-current_col) : 0 ; 
    e.rowBuff[current_row+1].b = (char*)malloc( sizeof(char) * (nextLineLen+1) );
    if(nextLineLen){ 
        memcpy(e.rowBuff[current_row+1].b ,e.rowBuff[current_row].b+current_col ,nextLineLen);
        e.rowBuff[current_row+1].b[nextLineLen] = '\0';
    }
    e.rowBuff[current_row+1].len = nextLineLen;
    
    // rewrite current line
    int currentLineLen = e.rowBuff[current_row].len - nextLineLen; 
    char * temp = e.rowBuff[current_row].b;
    e.rowBuff[current_row].b = (char*)realloc(e.rowBuff[current_row].b , sizeof(char) * (currentLineLen+1));
    if(currentLineLen) memcpy(e.rowBuff[current_row].b ,temp ,currentLineLen);
    e.rowBuff[current_row].len = currentLineLen;
    if(currentLineLen) e.rowBuff[current_row].b[currentLineLen] = '\0';

    e.modification_num++;
}

void saveToDisk (){
    FILE* file = fopen(e.filePath,"w+");
    if (file == NULL) die("fopen") ;
    
    for (int i =0 ;i<e.rowsNum ;i++){
        if(e.rowBuff[i].len) write(fileno(file), e.rowBuff[i].b,e.rowBuff[i].len);

        if (i != e.rowsNum-1){
            write(fileno(file),"\n",1);
        }
    }
    e.modification_num = 0;
    fclose(file);
}
