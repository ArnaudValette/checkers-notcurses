#include <fcntl.h>
#include <unistd.h>

int debug_fd = -1;

void init_debug() {
  if (debug_fd == -1) {
    debug_fd = open("./debug", O_WRONLY);
  }
}

void debug_log(char *str) {
  if (debug_fd == -1) init_debug();
  int i = 0;
  for (; str[i] != '\0'; i++)
    ;
  write(debug_fd, str, i);
}
