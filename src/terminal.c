#include "utilities.h"

#define QUIT_ATTEMPTS 2 


int getWindowSize(int *rows, int *cols){
    struct winsize ws;
    if (ioctl(STDIN_FILENO, TIOCGWINSZ, &ws) == -1 || ws.ws_col == 0) {
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
    e.startingX = 0;

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
    int readStatus;
    char c = readKey(&readStatus);
    if (readStatus == 0) return;
    if (c != CTRL_KEY('q')) e.quit_attempts = 0;

    switch (c){
        case QUIT:
            if (e.quit_attempts == 0 && e.modification_num){
                char pop_up[] = "Warning ! File has unsaved changes. ";
                writeMessage(&e.message, pop_up, strlen(pop_up));
                e.quit_attempts ++ ;
                break;
            }

            write(STDOUT_FILENO ,"\x1b[2J\x1b[3J" ,8);
            write(STDOUT_FILENO , "\x1b[H" ,3);
            
            exit(0);
        
        case TAB :
            tab();
            break;
        case SAVE:{
            if (e.filePath == NULL){
                struct string newFile = editorPrompt("save as ");
                
                if (newFile.b == NULL){
                    freeMessage();
                    break;
                }
                
                e.filePath = malloc(newFile.len+1);
                memcpy(e.filePath ,newFile.b , newFile.len);
                pathToFileName(newFile.b);
                stringFree(&newFile);
            }
            char message[100];
            snprintf(message,sizeof(message),
                    (e.modification_num > 1) ? "%d modifications written to disk !" : "%d modification written to disk !"
                    ,e.modification_num
            );
            writeMessage(&e.message, message, strlen(message));
            saveToDisk();
            break;
        }
        case ENTER:
            enter();
            break;
        case BACKSPACE2:
        case BACKSPACE1:
            backspace(); 
            break;
        
        case ESCAPE :{
            char seq[5] ;
            if(read(STDIN_FILENO,&seq[0],1) == -1){break;}
            if(read(STDIN_FILENO,&seq[1],1) == -1){break;}
            switch(seq[0]){
                case '[':
                    switch (seq[1]){
                        case UP_ARROW:
                            upArrow();
                            break;
                        case DOWN_ARROW:
                            downArrow(); 
                            break;
                        case RIGHT_ARROW:
                            rightArrow();
                            break;
                        case LEFT_ARROW:
                            leftArrow();     
                            break;
                        case '1':
                            if(read(STDIN_FILENO,&seq[2],1) == -1){break;}
                            if(read(STDIN_FILENO,&seq[3],1) == -1){break;}
                            if(read(STDIN_FILENO,&seq[4],1) == -1){break;}
                            if (seq[2] == ';' && seq[3] == '5'){
                                switch (seq[4]){
                                    case LEFT_ARROW :
                                        gotoPrevWord();
                                        break;
                                    case RIGHT_ARROW :
                                        gotoNextWord();
                                        break;
                                    case UP_ARROW :
                                        break;
                                    case DOWN_ARROW :
                                        break;
                                    default :
                                        tcflush(STDIN_FILENO, TCIFLUSH);

                                }
                            }
                            else if (seq[2] == ';' && seq[3] == '3'){
                                 switch (seq[4]){
                                    case LEFT_ARROW :
                                        break;
                                    case RIGHT_ARROW :
                                        break;
                                    case UP_ARROW :
                                        moveLineUp();
                                        break;
                                    case DOWN_ARROW :
                                        moveLineDown();
                                        break;
                                    default :
                                        tcflush(STDIN_FILENO, TCIFLUSH);                                   

                                }

                            }
                            break;
                        default : 
                            tcflush(STDIN_FILENO, TCIFLUSH);
                    }
                    break;
                case DOLLAR_SIGN:
                    dollarSign();
                    break;
                case ZERO:
                    e.cx = 0;
                    e.coloff = 0;
                    break;
            }
        }
        default :
           character(c); 
    }
}
void die(const char* s){
    write(STDOUT_FILENO ,"\x1b[2J\x1b[3J" ,8);
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
    if (e.filePath != NULL)
        free(e.filePath);
    if (e.message.b != NULL)
        stringFree(&e.message);
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
struct string editorPrompt(char* prompt){
    int prevStartingX = e.startingX;
    e.startingX = 0;
    e.cx = strlen(prompt) ;
    e.cy = e.windowsLength+1;

    struct string returnInfo;
    initString(&returnInfo);
    
    struct string command;
    initString(&command);
    stringAppend(&command, prompt ,strlen(prompt));

    while(1){
        writeMessage(&e.message, command.b, command.len);
        refreshScreen(); 
        int readStatus;
        char c = readKey(&readStatus);
        if (readStatus == 0) continue;
        switch (c){
            case ENTER :
                e.cx= 0;
                e.cy =0;
                stringFree(&command);
                e.startingX = prevStartingX;
                return returnInfo;
            case ESCAPE :{
                char seq[2] = {'\0','\0'} ;
                if(read(STDIN_FILENO,&seq[0],1) == -1){break;}
                if(read(STDIN_FILENO,&seq[1],1) == -1){break;}
                switch(seq[0]){
                    case '[':
                        switch (seq[1]){
                            case RIGHT_ARROW :
                                if(command.len && e.cx != command.len-1 )e.cx++;
                                break;
                            case LEFT_ARROW :
                                if(e.cx) e.cx--;
                                break;
                        }
                        break;
                }
                break;
            }
            case CTRL_KEY('c'):
                free(returnInfo.b);
                returnInfo.b = NULL;
                returnInfo.len = 0;
                e.cx = 0;
                e.cy = 0;
                stringFree(&command);
                e.startingX = prevStartingX;
                return returnInfo;
            case BACKSPACE1:
            case BACKSPACE2:
                if(e.cx != strlen(prompt)){
                    removeCharInRow(&returnInfo, e.cx-strlen(prompt));
                    removeCharInRow(&command, e.cx);
                    e.cx--;
                }
                break;
            default:{
                if (!iscntrl(c) && e.cx != e.windowsWidth-1){
                        insertCharInRow(&command,e.cx,c);
                        insertCharInRow(&returnInfo ,e.cx-strlen(prompt), c);
                        e.cx++;
                }
            }
        }
    }
    e.startingX = prevStartingX;
}
