#include <assert.h>
#include <stdio.h>
#include <string.h>
#include "queue.h"

#define LENGTH(x) (sizeof(x)/sizeof(*(x)))

static void test_reverse_empty();
static void test_reverse_single();
static void test_reverse();

static void test_sort_empty();
static void test_sort_single();
static void test_sort();

// Print out the index and the value of each element.
bool show_one(queue_element* elem, queue_function_args* args) {
  printf("Item %d == %d\n", *(int*) args, *(int*) elem);
  *(int*) args = *(int*) args + 1;
  return true;
}

int compare_ints(const void* a, const void* b) {
  return *(int*) a - *(int*) b;
}

int compare_elements(queue_element* a, queue_element* b) {
  return *(int*) a - *(int*) b;
}

static void print_queue(queue* q) {
  int index = 0;
  queue_apply(q, show_one, &index);
  printf("Queue size is %zu\n\n", queue_size(q));
}

int main(int argc, char* argv[]) {
  queue* q = queue_create();

  assert(queue_is_empty(q));

  int x = 0;
  int y = 1;
  int z = 2;

  int* elems[] = {&x, &y, &z, &x};
  int size = LENGTH(elems);

  int i;
  for (i = 0; i < size; i++) {
    queue_append(q, elems[i]);
    assert(!queue_is_empty(q));
  }

  print_queue(q);
  assert(size == queue_size(q));

  for (i = 0; i < size; i++) {
    assert(!queue_is_empty(q));

    int* ret;
    assert(queue_remove(q, (queue_element**) &ret));
    assert(ret == elems[i]);
    assert(size - 1 - i == queue_size(q));

    print_queue(q);
  }

  assert(queue_is_empty(q));

  int* ret;
  assert(!queue_remove(q, (queue_element**) &ret));

  free(q);

  test_reverse_empty();
  test_reverse_single();
  test_reverse();

  test_sort_empty();
  test_sort_single();
  test_sort();

  return 0;
}

static void test_reverse_empty() {
  queue* q = queue_create();

  queue_reverse(q);
  assert(queue_is_empty(q));

  free(q);
}

static void test_reverse_single() {
  queue* q = queue_create();

  int x = 10;
  queue_append(q, &x);
  queue_reverse(q);

  int *ret;
  queue_remove(q, (queue_element **) &ret);

  assert(x = *(int*) ret);

  assert(queue_is_empty(q));

  free(q);
}

static void test_reverse() {
  const int SIZE = 20;

  queue* q = queue_create();

  int* vals = malloc(sizeof(*vals) * SIZE);
  for (int i = 0; i < SIZE; i++) {
    vals[i] = i;
  }

  for (int i = 0; i < SIZE; i++) {
    queue_append(q, (queue_element*) (vals + i));
  }

  queue_reverse(q);
  print_queue(q);

  for (int i = SIZE - 1; i >= 0; i--) {
    int* ret;
    assert(queue_remove(q, (queue_element**) &ret));
    assert(vals[i] == *(int*) ret);
  }

  assert(queue_is_empty(q));

  free(q);
  free(vals);
}

static void test_sort_empty() {
  queue* q = queue_create();

  queue_sort(q, compare_elements);
  assert(queue_is_empty(q));

  free(q);
}

static void test_sort_single() {
  queue* q = queue_create();

  int x = 10;
  queue_append(q, &x);
  queue_sort(q, compare_elements);

  int *ret;
  queue_remove(q, (queue_element **) &ret);

  assert(x = *(int*) ret);

  assert(queue_is_empty(q));

  free(q);
}

static void test_sort() {
  int unsorted[] = { 2, 4, 3, 18, 6, 6, 5, 3 };
  int size = LENGTH(unsorted);
  int* sorted = malloc(size * sizeof(sorted[0]));
  memcpy(sorted, unsorted, sizeof(unsorted[0]) * size);
  qsort(sorted, size, sizeof(sorted[0]), compare_ints);

  queue* q = queue_create();

  for (int i = 0; i < size; i++) {
    queue_append(q, (queue_element*) (unsorted + i));
  }

  queue_sort(q, compare_elements);

  for (int i = 0; i < size; i++) {
    int *ret;
    queue_remove(q, (queue_element **) &ret);
    assert(sorted[i] == *(int*) ret);
  }

  free(q);
  free(sorted);
}
