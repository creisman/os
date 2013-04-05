/* Implements queue abstract data type. */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "queue.h"

/* Each link in the queue stores a queue_element and
 * a pointer to the next link in the queue. */
typedef struct _queue_link {
  queue_element* elem;
  struct _queue_link* next;
} queue_link;

/* This is the actual implementation of the queue struct that
 * is declared in queue.h. */
struct _queue {
  queue_link* head;
};

/* This is private helper method to reverse the queue. It takes in the queue
 * to traverse and the current place in the queue. It returns the node that
 * should go before the current node. */
static queue_link* queue_reverse_helper(queue* q, queue_link* current);

/* A helper function that does mergesort on the passed queue (uses queue_link
 * to avoid needing to allocate the queue struct). */
static queue_link* merge_sort(queue_link* q, queue_compare qc);

queue* queue_create() {
  queue* q = (queue*) malloc(sizeof(queue));

  if (!q) {
    // Failed to allocate memory.
    return NULL;
  }

  q->head = NULL;
  return q;
}

/* Private */
static queue_link* queue_new_element(queue_element* elem) {
  queue_link* ql = (queue_link*) malloc(sizeof(queue_link));

  if (!ql) {
    // Failed to allocate memory.
    return NULL;
  }

  ql->elem = elem;
  ql->next = NULL;

  return ql;
}

void queue_append(queue* q, queue_element* elem) {
  assert(q != NULL);

  queue_element* element = queue_new_element(elem);

  // BUG ONE. If it's empty, set head.
  if (q->head == NULL) {
    q->head = element;
  } else {  // If not empty, set at end.
    // Find the last link in the queue.
    queue_link* cur;
    for (cur = q->head; cur->next; cur = cur->next) {
    }

    // Append the new link.
    cur->next = element;
  }
}

bool queue_remove(queue* q, queue_element** elem_ptr) {
  queue_link* old_head;

  assert(q != NULL);
  assert(elem_ptr != NULL);
  if (queue_is_empty(q)) {
    return false;
  }

  *elem_ptr = q->head->elem;
  old_head = q->head;
  q->head = q->head->next;

  // BUG TWO. old_head is not freed.
  free(old_head);

  return true;
}

bool queue_is_empty(queue* q) {
  assert(q != NULL);
  return q->head == NULL;
}

/* private */
static bool queue_count_one(queue_element* elem, queue_function_args* args) {
  size_t* count = (size_t*) args;
  *count = *count + 1;
  return true;
}

size_t queue_size(queue* q) {
  size_t count = 0;
  queue_apply(q, queue_count_one, &count);
  return count;
}

bool queue_apply(queue* q, queue_function qf, queue_function_args* args) {
  assert(q != NULL && qf != NULL);

  if (queue_is_empty(q))
    return false;

  for (queue_link* cur = q->head; cur; cur = cur->next) {
    if (!qf(cur->elem, args))
      break;
  }

  return true;
}

void queue_reverse(queue* q) {
  assert(q != NULL);

  // If empty, do nothing.
  if (queue_is_empty(q)) {
    return;
  }

  queue_link* end = queue_reverse_helper(q, q->head);
  end->next = NULL;
}

static queue_link* queue_reverse_helper(queue* q, queue_link* current) {
  // If the last node, set as head and return.
  if (current->next == NULL) {
    q->head = current;
  } else {
    queue_link* prev = queue_reverse_helper(q, current->next);
    prev->next = current;
  }

  return current;
}

/* This is merge sort */
void queue_sort(queue* q, queue_compare qc) {
  q->head = merge_sort(q->head, qc);
}

static queue_link* merge_sort(queue_link* q, queue_compare qc) {
  if (q == NULL || q->next == NULL) {
    return q;
  }

  // Split in half efficiently by doing every other.
  queue_link *evens, *even_cur, *odds, *odd_cur;

  // Initialize the first two.
  evens = q;
  even_cur = evens;
  q = q->next;

  odds = q;
  odd_cur = odds;
  q = q->next;

  // Move the rest.
  int count = 0;
  while (q != NULL) {
    if (count % 2 == 0) {
      even_cur->next = q;
      even_cur = even_cur->next;
    } else {
      odd_cur->next = q;
      odd_cur = odd_cur->next;
    }

    q = q->next;

    count++;
  }

  // Terminate the two lists.
  even_cur->next = NULL;
  odd_cur->next = NULL;

  // Recursively sort.
  evens = merge_sort(evens, qc);
  odds = merge_sort(odds, qc);

  // Merge results.
  queue_link* cur;
  while (evens != NULL && odds != NULL) {
    queue_link* to_add;
    if (qc(evens->elem, odds->elem) <= 0) {
      to_add = evens;
      evens = evens->next;
    } else {
      to_add = odds;
      odds = odds->next;
    }

    if (q == NULL) {
      q = to_add;
      cur = q;
    } else {
      cur->next = to_add;
      cur = cur->next;
    }
  }

  // cur will never be null here since evens and odds are never null initially.
  if (evens == NULL) {
    cur->next = odds;
  } else {
    cur->next = evens;
  }

  return q;
}

void queue_destroy(queue* q, bool free_entries) {
  queue_link* cur;
  queue_link* next;
  if (q != NULL) {
    cur = q->head;
    while (cur) {
      next = cur->next;

      if (free_entries) {
        free(cur->elem);
      }
      free(cur);

      cur = next;
    }
    free(q);
  }
}
