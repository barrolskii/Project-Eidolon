#include "hashtable.h"

static uint32_t hash(const char *key, int len)
{
    /* FNV-1a hashing algorithm */
    uint32_t hash = 2166136261u;

    for (int i = 0; i < len; i++)
    {
        hash ^= key[i];
        hash *= 16777619;
    }

    return hash % TABLE_SIZE;
}

struct hash_table *ht_init()
{
    struct hash_table *ht = malloc(sizeof(struct hash_table));
    ht->items = calloc(TABLE_SIZE, sizeof(struct ht_item*));
    ht->count = 0;

    return ht;
}

static void free_chain(struct ht_item *item)
{
    if (item->next) free_chain(item->next);

    /* The key is heap allocated so we must free that */
    free(item->key);
    free(item);
}

void ht_free(struct hash_table *ht)
{
    for (int i = 0; i < TABLE_SIZE; i++)
    {
        if (!ht->items[i]) continue;

        /* The key is heap allocated so we must free that */
        /* We don't free the value as the cleanup there is */
        /* dealt by the garbage collector */
        free(ht->items[i]->key);
        //free(ht->items[i]->value);

        if (ht->items[i]->next) free_chain(ht->items[i]->next);

        free(ht->items[i]);
    }

    free(ht->items);
    free(ht);
}

int ht_contains_key(struct hash_table *ht, char *key)
{
    unsigned index = hash(key, strlen(key));

    /* Quick and dirty fix. If there is no entry at the table then return 0 */
    if (!ht->items[index]) return 0;
    if (strcmp(ht->items[index]->key, key) == 0) return 1;

    /* If the first key did not match then check the chain */
    struct ht_item *curr = ht->items[index]->next;
    while (curr)
    {
        if (strcmp(curr->key, key) == 0) return 1;

        curr = curr->next;
    }

    return 0;
}

static struct ht_item *new_item(struct hash_table *ht, char *key, object_t *value)
{
    struct ht_item *item = malloc(sizeof(struct ht_item));
    item->next = NULL;
    item->key = key;
    item->value = value;

    return item;
}

int ht_insert(struct hash_table *ht, char *key, object_t *value)
{
    unsigned index = hash(key, strlen(key));

    /* If the index is empty then add a new item at that index */
    if (!ht->items[index])
    {
        ht->items[index] = new_item(ht, key, value);
        return 0;
    }

    /* If the index is not empty then append the new item to the list at that index */
    struct ht_item *curr = ht->items[index];
    while(curr->next)
    {
        curr = curr->next;
    }

    curr->next = new_item(ht, key, value);

    return 0;
}

int ht_update_key(struct hash_table *ht, char *key, object_t *value)
{
    unsigned index = hash(key, strlen(key));

    /* Ideally this should never happen but just in case */
    /* return 0 if the index is NULL */
    if (!ht->items[index]) return 0;

    ht->items[index]->value = value;

    return 1;
}

object_t *ht_get_value(struct hash_table *ht, char *key)
{
    if (!ht_contains_key(ht, key)) return NULL;

    unsigned index = hash(key, strlen(key));

    return ht->items[index]->value;
}
