#include "utilities.h"

int readKey(char *buf, size_t cap, int *len) {
    unsigned char c;
    *len = 0;

    int n = read(STDIN_FILENO, &c, 1);
    if (n == 0) return -1;                       // timeout, no input
    if (n < 0) return (errno == EAGAIN) ? 0 : -1;
    log_message(logger, &c, 1, INFO);

    buf[(*len)++] = c;

    if (c == 0x1b) {                            // escape sequence
        unsigned char next;
        // VTIME timeout makes a lone ESC distinguishable
        if (read(STDIN_FILENO, &next, 1) != 1) return 1;   // plain ESC
        log_message(logger, buf+*len, 1, 1);
        buf[(*len)++] = next;
        if (next == '[' || next == 'O') {
            while (*len < (int)cap) {
                if (read(STDIN_FILENO, &next, 1) != 1) break;
                log_message(logger, buf+*len, 1, 1);
                buf[(*len)++] = next;
                if (next >= 0x40 && next <= 0x7e) break;   // CSI final byte
            }
        }
        return 1;
    }

    int extra = 0;                              // UTF-8
    if      ((c & 0xE0) == 0xC0) extra = 1;
    else if ((c & 0xF0) == 0xE0) extra = 2;
    else if ((c & 0xF8) == 0xF0) extra = 3;

    while (extra-- > 0 && *len < (int)cap) {
        if (read(STDIN_FILENO, &c, 1) != 1) break;
        log_message(logger, buf+*len, 1, 1);
        buf[(*len)++] = c;
    }
    return 1;
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
void readFile(char* file) {
    log_message(logger, file, strlen(file), INFO);
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
