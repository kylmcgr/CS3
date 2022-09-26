#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>
#include <string.h>
#include <stdint.h>
#include "match.h"

void check(bool actual, char *pattern, char *text){
  char *array = malloc(sizeof(char)*(strlen(pattern)+strlen(text)+100));
  array[0] = '\0';
  strcat(array, "echo \"");
  strcat(array, text);
  strcat(array, "\" | grep -c \"^");
  strcat(array, pattern);
  strcat(array, "$\"");
  FILE *file = popen(array, "r");
  assert(file != NULL);
  char buffer[100];
  fread(buffer, 1, 1, file);
  assert((buffer[0] == '1' && actual == true) || (buffer[0] == '0' && actual == false));
  pclose(file);
  free(array);
}

bool is_valid(char *string, size_t len){
  for (size_t i = 0; i < len; i++){
    if ((string[i] >= 'a' || string[i] <= 'z')){// || (string[i] >= 'A' && string[i] <= 'Z')){
      if (string[i] != '*' && string[i] != '.'){
        return false;
      }
      if (i < strlen(string) - 1 && string[i] == '*' &&  string[i + 1] == '*'){
        return false;
      }
    }
  }
  return true;
}

int LLVMFuzzerTestOneInput(uint8_t *data, size_t size) {
    size_t intsize = size / sizeof(int8_t);
    uint8_t *intdata = (uint8_t *)data;
    if (intsize >= 2) {
        size_t len1 = intdata[0];
        if (size >= sizeof(uint8_t) + len1) {
            size_t len2 = size - sizeof(uint8_t) - len1;
            char *str = ( (char *)intdata ) + 1;
            char *str1 = calloc(len1 + 1, sizeof(char));
            char *str2 = calloc(len2 + 1, sizeof(char));
            strncpy(str1, str, len1);
            strncpy(str2, str + len1, len2);
            if (is_valid(str1, len1) && is_valid(str2, len2))  {
                printf("s1=%s, s2=%s\n", str1, str2);
                bool result = match(str1, str2);
                check(result, str1, str2);
                free(str1);
                free(str2);
                return result;
            }
            free(str1);
            free(str2);
        }
    }
    return 0;
}
