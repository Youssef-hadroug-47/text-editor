#ifndef UTILIS
#define UTILIS



/// Includes ///
#include <math.h>
#include <sys/wait.h>
#include <termios.h>
#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <string.h>
#include <sys/types.h>
#include <stddef.h>
#include <time.h> 
#include <fcntl.h>
/// structures ///

#define CTRL_KEY(k) ((k)-'a'+1)

#define COLOR_LIST(X)            \
    X(COLOR_BLACK,   "\x1b[30m") \
    X(COLOR_RED,     "\x1b[31m") \
    X(COLOR_GREEN,   "\x1b[32m") \
    X(COLOR_YELLOW,  "\x1b[33m") \
    X(COLOR_BLUE,    "\x1b[34m") \
    X(COLOR_MAGENTA, "\x1b[35m") \
    X(COLOR_CYAN,    "\x1b[36m") \
    X(COLOR_WHITE,   "\x1b[37m")

#define AS_ENUM(name, code) name,
#define AS_CODE(name, code) [name] = code,
#define ANSI_RESET "\x1b[0m"

enum color { COLOR_LIST(AS_ENUM) COLOR_COUNT };
const char *color_to_ansi(enum color c);



struct string{
    char* b;
    int len;
    int lenByte;
};


enum messageType {
  WARNING,
  INFO,
  ERROR,
  SUCCESS
};


extern FILE* logger; 
struct editorConfig {
    int cx,cy;
    int windowsLength;
    int windowsWidth;
    int startingX;

    int rowoff;
    int coloff;

    struct termios original_term;
    
    char* filename;
    char* filePath;
    
    int modification_num;

    struct string *rowBuff;
    int rowsNum;
    
    struct string startOfLineChar;

    int quit_attempts;
};

struct FLASH_MESSAGE {
  enum color color;
  struct string message;
  int messageTime ;
  int messageWait ;
};
extern struct FLASH_MESSAGE Flash_Message;

enum editorKey {

    CHARACTER = 3000,
    CTRL_RIGHT_ARROW = 2003,
    CTRL_LEFT_ARROW = 2004,
    ALT_ARROW_UP = 1001,
    ALT_ARROW_DOWN = 1002,
    ALT_ZERO = 1000 + '0',
    ALT_DOLLAR_SIGN = 1000 + '$',
    LEFT_ARROW = 'D', 
    RIGHT_ARROW = 'C',
    UP_ARROW = 'A',
    DOWN_ARROW = 'B',
    DOLLAR_SIGN = '$',
    ZERO = '0',
    BACKSPACE1 = 127,
    BACKSPACE2 = 8,
    ENTER = 13,
    QUIT = CTRL_KEY('q'),
    SAVE = CTRL_KEY('s'),
    CTRL_C = CTRL_KEY('c'),
    ESCAPE = 27,
    TAB = 9
};
extern struct editorConfig e;

/// Logging ///
int log_message (FILE* log, char* message, int len, enum messageType messagetype);


/// Terminal ///
void get_editor_key_name(enum editorKey key, char **out_str, size_t *out_len);
int handleKeys(enum editorKey key, const char* buff, const int len);
void enableRawMode();
void disableRawMode();
void die(const char* s);
int getWindowSize(int* rows , int* cols);
void initEditorConfig();
void exiting();
int utf8_len(unsigned char c);
int createEvent(const char* entryBuffer, int len);
struct string editorPrompt(char* prompt);
/// Output ///
void refreshScreen();
void drawRows(struct string *ab);
void drawStatusLine(struct string *ab);
void drawMessage(struct string *ab , struct string message);
void writeMessage(char* message , int len);
void drawEditorName(struct string *ab);
/// Input ///
int readKey(char* buff, size_t cap, int* len);
void pathToFileName(char* path);
void readFile(char* file);

/// buffer append ///
void initString(struct string *ab);
void clearString(struct string *ab);
void stringAppend(struct string *ab , const char* c , int len);
void stringFree(struct string *ab);

/// editing ////
int getLen(int at , char* input );
int getPosInBytes(int at , char* input , int len);
void insertCharInRow(struct string* ab ,int at ,char* input , int inputLength);
void insertChar(char* input , int inputLength);
void removeCharInRow(struct string* ab,int at , int len);
int removeChar();
void saveToDisk();
void insertNewLine();

/// KeyBinding ///
void quit(int* quit_attempts, int number_of_modifications);
void leftArrow();
void rightArrow();
void upArrow();
void downArrow();
void backspace();
void enter();
void character(char* input , int inputLength);
void gotoEndOfLine();
void gotoBeginningOfLine();
void tab();
void gotoPrevWord();
void gotoNextWord();
void moveLineUp();
void moveLineDown();

#endif
