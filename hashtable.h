#ifndef __PHANTOM_HASHTABLE_H_
#define __PHANTOM_HASHTABLE_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "object.h"

#define TABLE_SIZE 256 // TODO: Temporary fix. Set up resizing of table nicely

struct ht_item {
    struct ht_item *next;
    char *key;
    object_t *value;
};

struct hash_table {
    struct ht_item **items;
    unsigned count;
};

struct hash_table *ht_init();
void ht_free(struct hash_table *ht);

int ht_contains_key(struct hash_table *ht, char *key);
int ht_insert(struct hash_table *ht, char *key, object_t *value);
int ht_update_key(struct hash_table *ht, char *key, object_t *value);

#endif // __PHANTOM_HASHTABLE_H_
