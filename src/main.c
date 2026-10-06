#include "utilities.h"

struct editorConfig e;
FILE* logger;
#define LOG_FILE_PATH "./log/log.txt"

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
        default:               *out_str = "UNKNOWN";          *out_len = 7;  break;
    }
}


int main (int argc ,char* argv[]){
    
    atexit(exiting);

    if (argc > 2)
        die("Invalid argument !");
    
    initEditorConfig();
    enableRawMode();
    
    if (argc == 2){
        readFile(argv[1]);
    }
    logger = fopen(LOG_FILE_PATH, "a+");

    int len = 0;

    refreshScreen();
    
    struct string better_buff;
    while (1){
      len = 0;
      char buff[8];
      char* event;
      size_t event_len;
      // read key
      // create event
      // dispatch service
      // log event with status code
      
      if (!readKey(buff, &len)) continue;
      get_editor_key_name(createEvent(buff, len), &event, &event_len);
      log_message(logger, event, event_len, INFO);
      handleKeys(buff, len);
      refreshScreen();
    }
    return 0;
}
