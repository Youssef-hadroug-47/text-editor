#include "utilities.h"

#define QUIT_ATTEMPTS 2 


void get_editor_key_name(enum editorKey key, char **out_str, size_t *out_len) {
    switch (key) {
        case CHARACTER:        *out_str = "CHARACTER";        *out_len = 9;  break;
        case CTRL_RIGHT_ARROW: *out_str = "CTRL_RIGHT_ARROW"; *out_len = 16; break;
        case CTRL_LEFT_ARROW:  *out_str = "CTRL_LEFT_ARROW";  *out_len = 15; break;
        case ALT_ARROW_UP:     *out_str = "ALT_ARROW_UP";     *out_len = 12; break;
        case ALT_ARROW_DOWN:   *out_str = "ALT_ARROW_DOWN";   *out_len = 14; break;
        case LEFT_ARROW:       *out_str = "LEFT_ARROW";       *out_len = 10; break;
        case RIGHT_ARROW:      *out_str = "RIGHT_ARROW";      *out_len = 11; break;
        case UP_ARROW:         *out_str = "UP_ARROW";         *out_len = 8;  break;
        case DOWN_ARROW:       *out_str = "DOWN_ARROW";       *out_len = 10; break;
        case DOLLAR_SIGN:      *out_str = "DOLLAR_SIGN";      *out_len = 11; break;
        case ZERO:             *out_str = "ZERO";             *out_len = 4;  break;
        case BACKSPACE1:       *out_str = "BACKSPACE1";       *out_len = 10; break;
        case BACKSPACE2:       *out_str = "BACKSPACE2";       *out_len = 10; break;
        case ENTER:            *out_str = "ENTER";            *out_len = 5;  break;
        case QUIT:             *out_str = "QUIT";             *out_len = 4;  break;
        case SAVE:             *out_str = "SAVE";             *out_len = 4;  break;
        case ESCAPE:           *out_str = "ESCAPE";           *out_len = 6;  break;
        case TAB:              *out_str = "TAB";              *out_len = 3;  break;
        case CTRL_C:           *out_str = "CTRL C";           *out_len = 6;  break;
        default:               *out_str = "UNKNOWN";          *out_len = 7;  break;
    }
}
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

    initString(&Flash_Message.message);
    Flash_Message.messageWait = 3;
}
int createEvent(const char* entryBuffer, int len) {
  if (!len) return -1;
  
  switch (entryBuffer[0]) {
    case TAB:
    case BACKSPACE1:
    case BACKSPACE2:
    case ENTER:
    case CTRL_C:
    case SAVE:
    case QUIT:
      return entryBuffer[0];
  
    case ESCAPE:
      if (len == 1) return ESCAPE;
      switch(entryBuffer[1]){
        case DOLLAR_SIGN: return ALT_DOLLAR_SIGN;
        case ZERO: return ALT_ZERO;
        case  '[':
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
      break;
    default:{
      if (utf8_len(entryBuffer[0]) != len) {
        char* error_message = "Invalid UTF-8 character";
        Flash_Message.color = COLOR_RED;
        writeMessage(error_message, strlen(error_message));
        log_message(logger, error_message, strlen(error_message), ERROR);
        return -1;
      }
      return !iscntrl(entryBuffer[0]) ? CHARACTER : -1 ;
    }
  }
  return -1;
}

int handleKeys(enum editorKey key, const char* buff, const int len) {

    if (!len) return 0;  
    if (key == -1) return -1;

    if (key != QUIT) e.quit_attempts = 0;
    switch (key){
        case QUIT:
          quit(&e.quit_attempts, e.modification_num);
          break;
        case TAB :
            tab();
            break;
        case SAVE:{
            if (!e.filePath){
                struct string newFile = editorPrompt("save as ");
                
                if (!newFile.b){
                    clearString(&Flash_Message.message);
                    break;
                }
                
                e.filePath = malloc( newFile.lenByte+1);
                memcpy( e.filePath, newFile.b, newFile.lenByte);
                pathToFileName( newFile.b);
                stringFree( &newFile);
            }
            char message[100];
            int messageLen = snprintf(message,sizeof(message),
                    (e.modification_num > 1) ? "%d modifications written to disk !" : "%d modification written to disk !"
                    ,e.modification_num
            );
            writeMessage(message, messageLen);
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
        case CTRL_LEFT_ARROW:
            gotoPrevWord();
            break;
        case CTRL_RIGHT_ARROW:
            gotoNextWord();
            break;
        case ALT_ARROW_DOWN:
            moveLineDown();
            break;
        case ALT_ARROW_UP:
            moveLineUp();
            break;
        case ALT_DOLLAR_SIGN:
            gotoEndOfLine();
            break;
        case ALT_ZERO:
            gotoBeginningOfLine();
            break;
        case CHARACTER :
            character((char*)buff, len);
            break;
        case ESCAPE:
            break;
        default:
            return -1;
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
    if (Flash_Message.message.b != NULL)
        stringFree(&Flash_Message.message);
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

void resetAtPromptExit(struct string* command , int prevStartingX){
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

  writeMessage(command.b, command.lenByte);
  refreshScreen();

  while(1) {

    char buff[8];
    int len = 0;
    char* event;
    size_t event_len;
    enum editorKey eventCode;

    if (readKey(buff, sizeof(buff), &len) != 1) 
      continue;

    eventCode = createEvent(buff, len);
    get_editor_key_name(eventCode, &event, &event_len);
    log_message(logger, event, event_len, INFO);
    
    switch (eventCode) {
      case ENTER :
        stringAppend(&returnInfo, command.b + promptLen, command.lenByte);
        resetAtPromptExit(&command, prevStartingX);
        return returnInfo;
      case RIGHT_ARROW:
      case CTRL_RIGHT_ARROW:
        if(command.len && e.cx+1 != command.len+1 ) e.cx++;
        break;
      case CTRL_LEFT_ARROW:
      case LEFT_ARROW :
        if(e.cx > promptLen) e.cx--;
        break;

      case CTRL_C:
        stringFree(&returnInfo);
        resetAtPromptExit(&command, prevStartingX);
        return returnInfo;
      case BACKSPACE1:
      case BACKSPACE2:{
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
      }
      case CHARACTER:{
        if (!iscntrl(buff[0]) && e.cx != e.windowsWidth-1){

          int posInBytes = getPosInBytes(e.cx, command.b, command.lenByte);
          insertCharInRow(&command, posInBytes, buff, len);
          e.cx++;
        }
        break;
      }
      default: continue;
    }

    if (len > 0 && event > 0) {
      writeMessage(command.b, command.lenByte);
      refreshScreen();
    }
  }
}
