#ifndef __PHANTOM_HASHTABLE_H_
#define __PHANTOM_HASHTABLE_H_

#include "object.h"

struct ht_item {
    struct ht_item *next;
    char *key;
    object_t value;
};

struct hash_table {
    struct ht_item **items;
};

struct hash_table *ht_init();
void ht_free(struct hash_table *ht);

int ht_contains_key(struct hash_table *ht, const char *key);
int ht_insert(struct hash_table *ht, const char *key, struct ht_item value); // TODO: Pointer to a ht_item?

#endif // __PHANTOM_HASHTABLE_H_
