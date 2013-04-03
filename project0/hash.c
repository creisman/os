/* Implements the abstract hash table. */

#include <assert.h>
#include <stdlib.h>

#include "hash.h"

/* Using an entry of this type is just a suggestion. You're
 * free to change this. */
typedef struct _hash_entry {
  void* key;
  void* value;
  struct _hash_entry* next;
} hash_entry;

struct _hash_table {
  hash_compare *comp;
  hash_hasher *hash;
  size_t len;
  size_t size;
  hash_entry **array;
};

static const int DEFAULT_SIZE = 10;
static const double MAX_LOAD = 2.0;

static void resize();

hash_table* hash_create(hash_hasher hasher, hash_compare compare) {
  assert(hasher);
  assert(compare);
  
  hash_table* ht = (hash_table*) malloc(sizeof(hash_table));

  if (!ht) {
    // Failed to allocate memory.
    return NULL;
  }

  ht->hash = hasher;
  ht->comp = compare;
  ht->len = DEFAULT_SIZE;
  ht->size = 0;
  ht->array = (hash_entry**) calloc(ht->len, sizeof(hash_entry*));

  return ht;
}

void hash_insert(hash_table* ht, void* key, void* value,
                 void** removed_key_ptr, void** removed_value_ptr) {
  assert(ht);
  assert(key);
  assert(value);

  uint64_t hash = ht->hash(key);

  hash_entry *bucket = ht->array[hash % ht->len];

  while (bucket != NULL && ht->comp(bucket->key, key) != 0) {
    bucket = bucket->next;
  }

  // I need to insert it at the front.
  if (bucket == NULL) {
    hash_entry *entry = (hash_entry*) malloc(sizeof(hash_entry));
    assert(entry);
    entry->key = key;
    entry->value = value;
    entry->next = ht->array[hash % ht->len];
    
    ht->array[hash % ht->len] = entry;
    ht->size = ht->size + 1;
  } else {  // I need to replace the values
    *removed_key_ptr = bucket->key;
    *removed_value_ptr = bucket->value;

    bucket->key = key;
    bucket->value = value;
  }

  if (((double) ht->size) / ht->len >= MAX_LOAD) {
    resize(ht);
  }
}

static void resize(hash_table *ht) {
  hash_entry **old = ht->array;
  size_t old_len = ht->len;
  ht->len = ht->len * 2;
  ht->array = (hash_entry **) calloc(ht->len, sizeof(hash_entry*));
  assert(ht->array);

  // Reinsert all the values.
  for (size_t i = 0; i < old_len; i++) {
    if (old[i] != NULL) {
      hash_entry *bucket = old[i];

      while (bucket != NULL) {
        // There are no duplicates in a rehash, so just add to front.
        uint64_t hash = ht->hash(bucket->key);
        hash_entry *entry = bucket;
        bucket = bucket->next;
        
        entry->next = ht->array[hash % ht->len];
        ht->array[hash % ht->len] = entry;
      }
    }
  }

  free(old);
}

bool hash_lookup(hash_table* ht, const void* key, void** value_ptr) {
  assert(ht);
  assert(key);

  uint64_t hash = ht->hash(key);

  hash_entry *bucket = ht->array[hash % ht->len];

  while (bucket != NULL && ht->comp(bucket->key, key) != 0) {
    bucket = bucket->next;
  }

  if (bucket == NULL) {
    return false;
  }

  *value_ptr = bucket->value;
  return true;
}

bool hash_is_present(hash_table* ht, const void* key) {
  void *tmp;
  return hash_lookup(ht, key, &tmp);
}

bool hash_remove(hash_table* ht, const void* key,
                 void** removed_key_ptr, void** removed_value_ptr) {
  assert(ht);
  assert(key);

  uint64_t hash = ht->hash(key);

  hash_entry *bucket = ht->array[hash % ht->len];

  hash_entry *prev = NULL;
  while (bucket != NULL && ht->comp(bucket->key, key) != 0) {
    prev = bucket;
    bucket = bucket->next;
  }

  if (bucket == NULL) {
    return false;
  }

  // It was the first value.
  if(prev == NULL) {
    ht->array[hash % ht->len] = bucket->next;
  } else {  // It was deeper in the list.
    prev->next = bucket->next;
  }

  *removed_key_ptr = bucket->key;
  *removed_value_ptr = bucket->value;
  free(bucket);

  return true;
}

void hash_destroy(hash_table* ht, bool free_keys, bool free_values) {
  for (size_t i = 0; i < ht->len; i++) {
    if (ht->array[i] != NULL) {
      hash_entry *bucket = ht->array[i];

      while (bucket != NULL) {
        hash_entry *tmp = bucket;
        bucket = bucket->next;

        if (free_keys) {
          free(tmp->key);
        }

        if (free_values) {
          free(tmp->value);
        }

        free(tmp);
      }
    }
  }

  free(ht->array);
  free(ht);
}
