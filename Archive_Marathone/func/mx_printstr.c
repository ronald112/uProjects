#include <unistd.h>

int mx_strlen(const char *s);

void mx_printstr(const char *s) {
  
  for (int counter = 0; counter < mx_strlen(s); counter++) {
    write (1, &s[counter], 1);
  }
}
