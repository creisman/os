#include <assert.h>
#include <stdio.h>
#include "queue.h"

#define LENGTH(x) (sizeof(x)/sizeof(*(x)))

// Print out the index and the value of each element.
bool show_one(queue_element* elem, queue_function_args* args) {
  printf("Item %d == %d\n", *(int*) args, *(int*) elem);
  *(int*) args = *(int*) args + 1;
  return true;
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

  free(q);

  return 0;
} 
