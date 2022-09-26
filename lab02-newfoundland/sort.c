#include "sort.h"

void ascending_int_sort_whole(int arr[], size_t nelements) {
  ascending_int_sort(arr, 0, nelements);
}

void ascending_int_sort(int arr[], size_t lo, size_t hi) {
  int_sort(arr, lo, hi, int_asc);
}

void descending_int_sort(int arr[], size_t lo, size_t hi) {
  int_sort(arr, lo, hi, int_desc);
}

void int_sort(int arr[], size_t lo, size_t hi, int_comparator_t compare) {
  for (size_t i = lo; i < hi; i++) {
    size_t desired = i;
    for (size_t j = i; j < hi; j++) {
      if (compare(arr[j], arr[desired]) < 0) {
        desired = j;
      }
    }
    int save = arr[i];
    arr[i] = arr[desired];
    arr[desired] = save;
  }
}

void string_sort(char *arr[], size_t lo, size_t hi, string_comparator_t compare) {
  sort((void *) arr, lo, hi, (comparator_t) compare);
}

void sort(void *arr[], size_t lo, size_t hi, comparator_t compare) {
  for (size_t i = lo; i < hi; i++) {
    size_t desired = i;
    for (size_t j = i; j < hi; j++) {
      if (compare(arr[j], arr[desired]) < 0) {
        desired = j;
      }
    }
    void *save = arr[i];
    arr[i] = arr[desired];
    arr[desired] = save;
  }
}
