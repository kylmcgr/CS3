#include <stdio.h>
#include <stdlib.h>

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

int num_lines(FILE *f){
  int lines = 0;
  char *result = malloc(sizeof(char));
  check_memory(result);
  while (1) {
    fread(result, 1, 1, f);
    if (feof(f)){
      break;
    }
    if (*result == '\n'){
      lines++;
    }
  }
  free(result);
  return lines;
}

int main(int argc, char *argv[]) {
  check_usage(argc, argv[0]);
  FILE *f = fopen(argv[1], "r");
  if (f == NULL) {
    printf("wc: %s: No such file or directory\n", argv[1]);
    return 1;
  }
  printf("%d %s\n", num_lines(f), argv[1]);
  fclose(f);
  return 0;
}
