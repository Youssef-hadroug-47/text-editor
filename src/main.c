#include "utilities.h"

/////* Global Data*////
struct editorConfig {
    struct termios original_term;
};
struct editorConfig e;
#define CTRL_KEY(k) ((k)-'a'+1)



///*Terminal*///
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

void handleKeys(){
    char c = readKey();
    
    switch (c){
        case CTRL_KEY('q'):
            write(STDOUT_FILENO ,"\x1b[2J" ,4);
            write(STDOUT_FILENO , "\x1b[H" ,3);
            
            exit(0);
            break;
    }
}

void die(const char* s){
    write(STDOUT_FILENO ,"\x1b[2J" ,4);
    write(STDOUT_FILENO , "\x1b[H" ,3);

    perror(s);
    exit(1);
}

void disableRawMode(){
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH,&e.original_term) == -1) die("tcsetattr");
}

void enableRawMode(){
    if (tcgetattr(STDIN_FILENO, &e.original_term) == -1) die("tcgetattr");
    atexit(disableRawMode);

    struct termios raw = e.original_term;
    raw.c_iflag &= ~(IXON | ICRNL | BRKINT | INPCK | ISTRIP);
    raw.c_lflag &= ~(ECHO | ICANON | ISIG | IEXTEN );
    raw.c_oflag &= ~(OPOST);
    raw.c_cflag |=(CS8);
    raw.c_cc[VMIN]=0;
    raw.c_cc[VTIME]=1;
    if (tcsetattr(STDIN_FILENO,TCSAFLUSH,&raw) == -1) die("tcsetattr");
}
/// Output ///

void refreshScreen(){
    write(STDOUT_FILENO ,"\x1b[2J" ,4);
    write(STDOUT_FILENO , "\x1b[H" ,3);
    
    drawRows();
    write(STDOUT_FILENO , "\x1b[H",3);
}

void drawRows(){
    for (int y =0;y<24;y++)
        write(STDOUT_FILENO , "~\r\n",3);
}

/// Input ////
char readKey(){
    char c ;
    int err;
    while( (err = read(STDIN_FILENO,&c,sizeof(char)) != 1 )){
        if (err == -1 && errno != EAGAIN)
            die("read");
    }
    return c;
}

int main (){
    enableRawMode();

    while (1){
        refreshScreen();
        handleKeys();
    }
    return 0;
}
