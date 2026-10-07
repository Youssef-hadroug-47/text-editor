#include "utilities.h"

int readKey(char* buff, int* len){
    *len = 0;
    int readStatus;
    char c;
    while ((readStatus = read(STDIN_FILENO, &c, sizeof(char))) == 1) {
      char code[4];
      int code_len = snprintf(code, sizeof(code), "%d", c);
      log_message(logger, code, code_len, INFO);
      if (sizeof(buff) < *len) return -1;
      memcpy(buff + *len, &c, 1);
      (*len)++;
    }

    if (readStatus == -1 && errno != EAGAIN) return -1;
    return *len > 0 ? 1 : -1;
}
void pathToFileName(char* path){
    int i = strlen(path);
    while(i >= 0 && path[i] != '/') i--;
    if (path[i] == '/') i++;
    i = (i == -1) ? 0 : i ;
    
    e.filename = (char*)malloc(strlen(path)-i+1);
    memcpy(e.filename, path+i , strlen(path)-i);
    e.filename[strlen(path)-i]= '\0';
}
void readFile(char* file){
    FILE* File = fopen(file,"r");
    if (File == NULL){
        die("fopen");
    }
    
    e.filePath = malloc(strlen(file)+1);
    memcpy(e.filePath,file,strlen(file));
    
    e.modification_num = 0;
    pathToFileName(file);
    

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
