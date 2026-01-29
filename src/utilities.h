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
struct string{
    char* b;
    int len;
};
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

    struct string message;
    int messageTime ;
    int messageWait ;
    FILE* tester;
    int quit_attempts;
};
enum editorKey{
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
    ESCAPE = 27,
    TAB = 9
};
extern struct editorConfig e;

/// Terminal ///
void handleKeys();
void enableRawMode();
void disableRawMode();
void die(const char* s);
int getWindowSize(int* rows , int* cols);
void initEditorConfig();
void exiting();
int utf8_len(unsigned char c);
struct string editorPrompt(char* prompt);
/// Output ///
void refreshScreen();
void drawRows(struct string *ab);
void drawStatusLine(struct string *ab);
void drawMessage(struct string *ab , struct string message);
void writeMessage(struct string *destination , char* message , int len);
void freeMessage();
void drawEditorName(struct string *ab);
/// Input ///
char readKey(int* readStatus);
void pathToFileName(char* path);
void readFile(char* file);

/// buffer append ///
void initString(struct string *ab);
void stringAppend(struct string *ab , const char* c , int len);
void stringFree(struct string *ab);

/// editing ////
void insertCharInRow(struct string* ab ,int at ,char* input , int inputLength);
void insertChar(char* input , int inputLength);
void removeCharInRow(struct string* ab,int at , int len);
int removeChar();
void saveToDisk();
void insertNewLine();

/// KeyBinding ///
void leftArrow();
void rightArrow();
void upArrow();
void downArrow();
void backspace();
void enter();
void character(char* input , int inputLength);
void dollarSign();
void tab();
void gotoPrevWord();
void gotoNextWord();
void moveLineUp();
void moveLineDown();
#endif
