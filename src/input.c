#include "utilities.h"

char readKey(){
    char c ;
    int err;
    while( (err = read(STDIN_FILENO,&c,sizeof(char)) != 1 )){
        if (err == -1 && errno != EAGAIN)
            die("read");
    }
    return c;
}

void readFile(char* file){
    FILE* File = fopen(file,"r");
    
    if (file == NULL){
        die("fopen");
    }
    
    
    int i = strlen(file);
    while(i >= 0 && file[i] != '/') i--;
    if (file[i] == '/') i++;
    i = (i == -1) ? 0 : i ;
    
    e.filename = (char*)malloc(strlen(file)-i+1);
    memcpy(e.filename, file+i , strlen(file)-i);
    e.filename[strlen(file)-i]= '\0';
    

    char* line = NULL;
    size_t len = 0 ;

    ssize_t linesize;

    while((linesize = getline(&line,&len,File)) != -1){
        while(linesize > 0 && (line[linesize-1] == '\r' || line[linesize-1] == '\n'))
            linesize--;

        e.rowBuff = (struct string*)realloc(e.rowBuff, sizeof(struct string) * (e.rowsNum+1));
        initString(e.rowBuff + e.rowsNum);
        stringAppend(e.rowBuff + e.rowsNum, line, linesize);
            
        
        e.rowsNum++;
    }
    free(line);
    fclose(File);
}
