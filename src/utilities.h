#ifndef UTILIS
#define UTILIS



/// Includes ///

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

/// structures ///

struct string{
    char* b;
    int len;
};
struct editorConfig {
    int cx,cy;
    int windowsLength;
    int windowsWidth;
    
    int rowoff;
    int coloff;

    struct termios original_term;
    
    struct string *rowBuff;
    int rowsNum;
};
enum editorKey{
    LEFT_ARROW, 
    RIGHT_ARROW,
    UP_ARROW,
    BOTTOM_ARROW
};
extern struct editorConfig e;

/// Terminal ///
void handleKeys();
void enableRawMode();
void disableRawMode();
void die(const char* s);
int getWindowSize(int* rows , int* cols);
void initEditorConfig();

/// Output ///
void refreshScreen();
void drawRows(struct string *ab);

/// Input ///
char readKey();
void readFile(char* file);

/// buffer append ///
void initString(struct string *ab);
void stringAppend(struct string *ab , const char* c , int len);
void stringFree(struct string *ab);


#endif
