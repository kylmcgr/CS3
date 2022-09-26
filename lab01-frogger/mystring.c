#include <stdlib.h>
#include <string.h>
#include "mystring.h"

strarray *strsplit(const char *str) {
  strarray *substrings = malloc(sizeof(strarray));
  substrings->data = malloc(1000 * sizeof(char*));
  substrings->data[0] = malloc(1000 * sizeof(char));
  int length = 0;
  int position = 0;
  int i = 0;
  while (str[i] != '\0'){
    if (str[i] != ' ') {
      substrings->data[length][position] = str[i];
      position++;
    } else if (i != 0 && str[i-1] != ' '){
      substrings->data[length][position] = '\0';
      length++;
      substrings->data[length] = malloc(1000 * sizeof(char));
      position = 0;
    }
    i++;
  }
  substrings->data[length][position] = '\0';
  if (strlen(substrings->data[length]) == 0){
    free(substrings->data[length]);
    length--;
  }
  substrings->length = length + 1;
  return substrings;
}
