#ifndef UTILIS
#define UTILIS
#include <termios.h>
#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ioctl.h>

/// Terminal ///
void handleKeys();
void enableRawMode();
void disableRawMode();
void die(const char* s);
int getWindowSize(int* rows , int* cols);
/// Output ///
void refreshScreen();
void drawRows();
/// Input ///
char readKey();

#endif
