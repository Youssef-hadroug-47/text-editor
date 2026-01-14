#include "utilities.h"
#include <stdio.h>

#define QUIT_ATTEMPTS 2 
#define CTRL_KEY(k) ((k)-'a'+1)

int getWindowSize(int *rows, int *cols){
    struct winsize ws;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1 || ws.ws_col == 0) {
        return -1;
    } 
    else {
        *cols = ws.ws_col;
        *rows = ws.ws_row;
        return 0;
    }
}
void initEditorConfig(){
    e.cx=0;
    e.cy=0;
    e.rowoff=0;
    e.coloff=0;
    if (getWindowSize(&e.windowsLength, &e.windowsWidth) == -1)
        die("getWindowSize");
    e.windowsLength -=2;
    e.rowsNum=0;
    e.rowBuff = NULL;

    e.filename = NULL;
    e.filePath = NULL;
   
    e.quit_attempts = 0;

    initString(&e.message);
    char message[]= "This text editor does not support UTF-8! UTF-8 characters may look broken !";
    writeMessage(&e.message, message,strlen(message));
    e.messageWait = 5;
}
void handleKeys(){
    char c = readKey();
    if (c != CTRL_KEY('q')) e.quit_attempts = 0;

    switch (c){
        case QUIT:
            if (e.quit_attempts == 0 && e.modification_num){
                char pop_up[] = "Warning ! File has unsaved changes. ";
                writeMessage(&e.message, pop_up, strlen(pop_up));
                e.quit_attempts ++ ;
                break;
            }

            write(STDOUT_FILENO ,"\x1b[2J" ,4);
            write(STDOUT_FILENO , "\x1b[H" ,3);
            
            exit(0);
        case DOLLAR_SIGN:
            dollarSign(); 
            break;
        case ZERO:
            e.cx= 0;
            e.coloff=0;
            break;
        case SAVE:{
            char message[100];
            snprintf(message,sizeof(message),
                    (e.modification_num) ? "%d modifications written to disk !" : "%d modification written to disk !"
                    ,e.modification_num
            );
            writeMessage(&e.message, message, strlen(message));
            saveToDisk();
            break;
        }
        case ENTER:
            enter();
            break;
        case BACKSPACE:
            backspace(); 
            break;
        
        case ESCAPE :{
            char seq[3] ;
            if(read(STDIN_FILENO,&seq[0],1) == -1){}
            if(read(STDIN_FILENO,&seq[1],1) == -1){}
            switch(seq[0]){
                case '[':
                    switch (seq[1]){
                        case 'A':
                            upArrow();
                            break;
                        case 'B':
                            downArrow(); 
                            break;
                        case 'C':
                            rightArrow();
                            break;
                        case 'D':
                            leftArrow();     
                            break;
                        case '5':
                            if(read(STDIN_FILENO,&seq[2],1) == -1){}
                            if (seq[2] == '~')
                                e.cx=0;
                        case '6':
                            if(read(STDIN_FILENO,&seq[2],1) == -1){}
                            if (seq[2] == '~')
                                e.cx = e.windowsLength -1;
                    }
            }
        }
        default :
           character(c); 
    }
}
void die(const char* s){
    write(STDOUT_FILENO ,"\x1b[2J" ,4);
    write(STDOUT_FILENO , "\x1b[H" ,3);
    perror(s);
    exit(1);
}
void exiting(){
    if (e.rowBuff != NULL){
        for (int i = 0 ; i<e.rowsNum ; i++){
            stringFree(&e.rowBuff[i]); 
        }
        free(e.rowBuff);
    }
    if (e.filename != NULL)
        free(e.filename);
    disableRawMode();
}

void disableRawMode(){
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH,&e.original_term) == -1) die("tcsetattr");
}
void enableRawMode(){
    if (tcgetattr(STDIN_FILENO, &e.original_term) == -1) die("tcgetattr");

    struct termios raw = e.original_term;
    raw.c_iflag &= ~(IXON | ICRNL | BRKINT | INPCK | ISTRIP);
    raw.c_lflag &= ~(ECHO | ICANON | ISIG | IEXTEN );
    raw.c_oflag &= ~(OPOST);
    raw.c_cflag |=(CS8);
    raw.c_cc[VMIN]=0;
    raw.c_cc[VTIME]=1;
    if (tcsetattr(STDIN_FILENO,TCSAFLUSH,&raw) == -1) die("tcsetattr");
}
