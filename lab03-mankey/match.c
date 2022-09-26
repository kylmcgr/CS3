#include "match.h"
#include <string.h>

bool match(char *pattern, char *text) {
  if (strlen(text) == 0){
    return (strlen(pattern) == 0 || (pattern[1] == '*' && match(pattern + 2, text)));
  }
  if (strlen(pattern) == 0){
    return strlen(text) == 0;
  }
  if (pattern[1] == '*'){
    if (pattern[0] == '.'){
      if (match(pattern, text + 1)){
        return true;
      } else {
        return match(pattern + 2, text);
      }
    }
    else if (pattern[0] == text[0]){
      if (text[1] == text[0]){
        return match(pattern, text + 1);
      } else {
        return match(pattern + 2, text + 1);
      }
    } else {
      return match(pattern + 2, text);
    }
  }
  if (pattern[0] == text[0] || pattern[0] == '.'){
    return match(pattern + 1, text + 1);
  }
  return false;
}
