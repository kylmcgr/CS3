#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void check_usage(int num_args, char *name){
  if (num_args != 2){
    printf("Usage: %s <filename>\n", name);
    exit(1);
  }
}

void check_memory(char *var){
  if (var == NULL) {
    printf("no memory");
    exit(1);
  }
}


char *get_longest(FILE *f){
  char *largest = malloc(101*sizeof(char));
  largest[0] = '\0';
  char *current = malloc(101*sizeof(char));
  char *result = malloc(sizeof(char));
  int curIdx = 0;
  check_memory(largest);
  check_memory(current);
  check_memory(result);
  while (1) {
    fread(result, 1, 1, f);
    current[curIdx] = *result;
    curIdx++;
    if (feof(f)){
      break;
    }
    if (*result == '\n' || *result == ' ') {
      current[curIdx-1] = '\0';
      if (strlen(current) > strlen(largest)) {
        strcpy(largest, current);
      }
      curIdx = 0;
    }
  }
  free(current);
  free(result);
  return largest;
}

int main(int argc, char *argv[]) {
  check_usage(argc, argv[0]);
  FILE *f = fopen(argv[1], "r");
  if (f == NULL) {
    printf("wc: %s: No such file or directory\n", argv[1]);
    return 1;
  }
  char *largest = get_longest(f);
  if (strlen(largest) != 0) {
    printf("%s\n", largest);
  }
  free(largest);
  fclose(f);
  return 0;
}
