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
int utf8_len(unsigned char c) {
    if ((c & 0x80) == 0) return 1;
    if ((c & 0xE0) == 0xC0) return 2;
    if ((c & 0xF0) == 0xE0) return 3;
    if ((c & 0xF8) == 0xF0) return 4;
    return -1;
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
  
    initString(&e.startOfLineChar);
    stringAppend(&e.startOfLineChar, "\e[38;5;25m", 10);
    stringAppend(&e.startOfLineChar, "🟆" , 4);
    stringAppend(&e.startOfLineChar , "\e[0m" , 4);

    e.quit_attempts = 0;

    initString(&e.message);
    e.messageWait = 5;
}
int createEvent(const char* entryBuffer, int len) {
  if (!len) return -1;
  
  switch (entryBuffer[0]) {
    case ESCAPE:
      break;
    case TAB:
    case BACKSPACE1:
    case BACKSPACE2:
    case ENTER:
    case SAVE:
    case QUIT:
      return entryBuffer[0];
    default: 
      return CTRL_KEY(entryBuffer[0]) ? CHARACTER : -1;
  }

  if (entryBuffer[1] == '['){
      switch (entryBuffer[2]){
        case UP_ARROW:
          return UP_ARROW;
        case DOWN_ARROW:
          return DOWN_ARROW;
        case RIGHT_ARROW:
          return RIGHT_ARROW;
        case LEFT_ARROW:
          return LEFT_ARROW;
        case '1':
          if (entryBuffer[3] == ';' && entryBuffer[4] == '5'){
            switch (entryBuffer[5]){
              case LEFT_ARROW :
                return CTRL_LEFT_ARROW;
              case RIGHT_ARROW :
                return CTRL_RIGHT_ARROW;
              case UP_ARROW :
              case DOWN_ARROW :
                return -1;
            }
          }
          else if (entryBuffer[3] == ';' && entryBuffer[4] == '3'){
            switch (entryBuffer[5]){
              case LEFT_ARROW :
              case RIGHT_ARROW :
                return -1;
              case UP_ARROW :
                return ALT_ARROW_UP;
              case DOWN_ARROW :
                return ALT_ARROW_DOWN;
            }
          }
          break;
      }
  }
  return -1;
}

int handleKeys(const char* buff, const int len) {

    if (!len) return 0;  

    if (buff[0] != CTRL_KEY('q')) e.quit_attempts = 0;
    switch (buff[0]){
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
                    clearString(&e.message);
                    break;
                }
                
                e.filePath = malloc(newFile.lenByte+1);
                memcpy(e.filePath ,newFile.b , newFile.lenByte);
                pathToFileName(newFile.b);
                stringFree(&newFile);
            }
            char message[100];
            int messageLen = snprintf(message,sizeof(message),
                    (e.modification_num > 1) ? "%d modifications written to disk !" : "%d modification written to disk !"
                    ,e.modification_num
            );
            writeMessage(&e.message, message, messageLen);
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
            switch(buff[1]){
              case '[':
                switch (buff[2]){
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
                    if (buff[3] == ';' && buff[4] == '5'){
                      switch (buff[5]){
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
                      }
                    }
                    else if (buff[3] == ';' && buff[4] == '3'){
                      switch (buff[5]){
                        case LEFT_ARROW :
                        case RIGHT_ARROW :
                          break;
                        case UP_ARROW :
                          moveLineUp();
                          break;
                        case DOWN_ARROW :
                          moveLineDown();
                          break;
                      }
                    }
                    break;
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
            break;
        }
        default :{
            character((char*)buff, len);

        }
    }
    return 0;
}
void die(const char* s){
  if (logger) fclose(logger);
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
    if (e.startOfLineChar.b != NULL)
        stringFree(&e.startOfLineChar);
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
    raw.c_cc[VTIME]=0;
    if (tcsetattr(STDIN_FILENO,TCSAFLUSH,&raw) == -1) die("tcsetattr");
}
void resetAtExit(struct string* command , int prevStartingX){
        e.cx= 0;
        e.cy =0;
        stringFree(command);
        e.startingX = prevStartingX;
}
struct string editorPrompt(char* prompt){
  int promptLen = strlen(prompt);
  int prevStartingX = e.startingX;
  e.startingX = 0;
  e.cx = promptLen ;
  e.cy = e.windowsLength+1;

  struct string returnInfo;
  initString(&returnInfo);

  struct string command;
  initString(&command);
  stringAppend(&command, prompt ,promptLen);

  while(1){
    writeMessage(&e.message, command.b, command.lenByte);
    refreshScreen();
    char buff[8];
    int len = 0;
    if(readKey(buff, &len) == -1) continue;
    switch (buff[0]){
      case ENTER :
        stringAppend(&returnInfo, command.b, command.lenByte);
        resetAtExit(&command, prevStartingX);
        return returnInfo;
      case ESCAPE :{
                     switch(buff[1]){
                       case '[':
                         switch (buff[2]){
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
                   stringFree(&returnInfo);
                   resetAtExit(&command, prevStartingX);
                   return returnInfo;
      case BACKSPACE1:
      case BACKSPACE2:
                   if(e.cx != promptLen ){
                     int posInBytes = getPosInBytes(e.cx, command.b , command.lenByte);
                     int charLen = utf8_len(command.b[getPosInBytes(e.cx - 1 , command.b , command.lenByte)]);
                     removeCharInRow(&command,
                         posInBytes, 
                         charLen
                         );
                     e.cx--;
                   }
                   break;
      default:{
                if (!iscntrl(buff[0]) && e.cx != e.windowsWidth-1){
                  int ascii_len = utf8_len(buff[0]);
                  if (ascii_len == -1){
                    resetAtExit(&command, prevStartingX);
                    clearString(&returnInfo);
                    stringAppend(&returnInfo , "Error !" , 7);
                    return returnInfo;
                  }


                  int posInBytes = getPosInBytes(e.cx, command.b, command.lenByte);
                  insertCharInRow(&command, posInBytes, buff, ascii_len);
                  e.cx++;
                }
              }
    }
  }
}
