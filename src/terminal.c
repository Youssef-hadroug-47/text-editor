#include "utilities.h"

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
    e.rowsNum=0;
    e.rowBuff = NULL;
}

void handleKeys(){
    char c = readKey();
    
    switch (c){
        case CTRL_KEY('q'):
            write(STDOUT_FILENO ,"\x1b[2J" ,4);
            write(STDOUT_FILENO , "\x1b[H" ,3);
            
            exit(0);
            break;
        case '$':
            e.cx= e.rowBuff[e.cy+e.rowoff].len-1;
            break;
        case '0':
            e.cx= 0;
            break;

        case 27 :{
            char seq[3] ;
            if(read(STDIN_FILENO,&seq[0],1) == -1){}
            if(read(STDIN_FILENO,&seq[1],1) == -1){}
            switch(seq[0]){
                case '[':
                    switch (seq[1]){
                        case 'A':
                            if(e.cy+e.rowoff -1 < e.rowsNum  && e.cy+e.rowoff != 0 && e.cx >= e.rowBuff[e.cy+e.rowoff].len-1) 
                                e.cx =  e.rowBuff[e.cy-1+e.rowoff].len-1 >= 0 ? 
                                        e.rowBuff[e.cy-1+e.rowoff].len-1 : 
                                        0 ;

                            else e.cx = 0;
                            if (e.cy) e.cy--; 
                            else if(e.rowoff) e.rowoff--;
                            break;
                        case 'B':
                            if(e.cy+e.rowoff+1 < e.rowsNum  && e.cx >= e.rowBuff[e.cy+e.rowoff].len-1){ 
                                e.cx  = (e.rowBuff[e.cy+1+e.rowoff].len > 0) ? 
                                        e.rowBuff[e.cy+1+e.rowoff].len-1 :
                                        0 ;
                            }
                            else e.cx = 0;
                            if(e.cy != e.windowsLength -1)  e.cy++;
                            else e.rowoff++;
                            break;
                        case 'C':
                            if(e.cy+e.rowoff<e.rowsNum  && e.cx == e.rowBuff[e.cy+e.rowoff].len -1) {
                                if (e.cy != e.windowsLength -1) e.cy++;
                                else e.rowoff++;
                                e.cx=0;
                            }
                            else if(e.cx != e.windowsWidth -1) e.cx++;
                            else e.coloff++;
                            break;
                        case 'D':
                            if (e.cx != 0) e.cx--;
                            else {
                                if (e.coloff) e.coloff--;
                                else if(e.cy+e.rowoff < e.rowsNum && e.cy+e.rowoff != 0){
                                        if (e.cy) e.cy--;
                                        else e.rowoff--;
                                        e.cx =  e.rowBuff[e.cy+e.rowoff].len-1 >= 0 ? 
                                                e.rowBuff[e.cy+e.rowoff].len-1 : 
                                                0 ;
                                }
                            }
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
