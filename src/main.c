#include "utilities.h"

struct editorConfig e;

int main (int argc ,char* argv[]){
    
    atexit(exiting);

    if (argc > 2)
        die("Invalid argument !");
    
    initEditorConfig();
    enableRawMode();
    
    if (argc == 2){
        readFile(argv[1]);
    }
    while (1){
        refreshScreen();
        handleKeys();
    }
    return 0;
}
