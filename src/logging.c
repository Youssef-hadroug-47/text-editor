#include "utilities.h"



int log_message (FILE* log, char* message, int len, enum messageType type) {

  if (!log || !message || !len )
    return -1;
  
  char messageType;
  switch (type) {
    case WARNING:
      messageType = 'W';
      break;
    case ERROR:
      messageType = 'E';
      break;
    case SUCCESS:
      messageType = 'S';
      break;
    case INFO:
    default:
      messageType = 'I';
      break;
  }


  time_t rawtime;
  time(&rawtime);
  struct tm* info = localtime(&rawtime);

  char currentDate[20];
  if(snprintf(currentDate, sizeof(currentDate), "%04d-%02d-%02d %02d:%02d:%02d", info->tm_year + 1900, info->tm_mon + 1, info->tm_mday, info->tm_hour, info->tm_min, info->tm_sec) < 0) return -1;

  if (fprintf(log, "[%s] %c: %.*s\r\n", currentDate, messageType, len, message) < 0) return -1;

  return fflush(log) == 0 ? 0 : -1;
}
