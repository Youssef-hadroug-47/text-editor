#include "utilities.h"

#define LOG_FILE_PATH "./log/log.txt"

struct editorConfig e;
FILE* logger;
struct FLASH_MESSAGE Flash_Message;


int main (int argc ,char* argv[]){
    
    atexit(exiting);
      
    if (argc > 2)
        die("Invalid argument !");

    logger = fopen(LOG_FILE_PATH, "a+");
    
    initEditorConfig();
    enableRawMode();
    
    if (argc == 2){
        readFile(argv[1]);
    }

    int buff_len = 0;
    enum editorKey eventCode;
    refreshScreen();
    
    struct string better_buff;
    while (1){
      buff_len = 0;
      char buff[8];
      char* event;
      size_t event_len;
      
      if (readKey(buff, sizeof(buff), &buff_len) == -1) continue;
      eventCode = createEvent(buff, buff_len);
      get_editor_key_name(eventCode, &event, &event_len);
      log_message(logger, event, event_len, INFO);
      handleKeys(eventCode, buff, buff_len);
      refreshScreen();
    }
    return 0;
}
