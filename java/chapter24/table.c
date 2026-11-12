//> Hash Tables table-c
#include <stdlib.h>
#include <string.h>

#include "memory.h"
#include "object.h"
#include "table.h"
#include "value.h"

//> max-load
#define TABLE_MAX_LOAD 0.75

static uint32_t hashValue(Value value) {
  if (IS_BOOL(value)) return AS_BOOL(value) ? 2 : 1;
  if (IS_NIL(value)) return 3;
  if (IS_NUMBER(value)) {
    uint64_t bits;
    double number = AS_NUMBER(value);
    if (number == 0) number = 0; // Normalize -0.0 and +0.0.
    memcpy(&bits, &number, sizeof bits);
    return (uint32_t)(bits ^ (bits >> 32));
  }
  if (IS_STRING(value)) return AS_STRING(value)->hash;
  return 0;
}

//< max-load
void initTable(Table* table) {
  table->count = 0;
  table->capacity = 0;
  table->entries = NULL;
}
//> free-table
void freeTable(Table* table) {
  FREE_ARRAY(Entry, table->entries, table->capacity);
  initTable(table);
}
//< free-table
//> find-entry
//> omit
// NOTE: The "Optimization" chapter has a manual copy of this function.
// If you change it here, make sure to update that copy.
//< omit
static Entry* findEntry(Entry* entries, int capacity,
                        Value key) {
/* Hash Tables find-entry < Optimization initial-index
  uint32_t index = key->hash % capacity;
*/
//> Optimization initial-index
  uint32_t index = hashValue(key) & (capacity - 1);
//< Optimization initial-index
//> find-entry-tombstone
  Entry* tombstone = NULL;
  
//< find-entry-tombstone
  for (;;) {
    Entry* entry = &entries[index];
/* Hash Tables find-entry < Hash Tables find-tombstone
*/
//> find-tombstone
    if (!entry->occupied) {
      if (!entry->tombstone) {
        // Empty entry.
        return tombstone != NULL ? tombstone : entry;
      } else {
        // We found a tombstone.
        if (tombstone == NULL) tombstone = entry;
      }
    } else if (valuesEqual(entry->key, key)) {
      // We found the key.
      return entry;
    }
//< find-tombstone

/* Hash Tables find-entry < Optimization next-index
    index = (index + 1) % capacity;
*/
//> Optimization next-index
    index = (index + 1) & (capacity - 1);
//< Optimization next-index
  }
}
//< find-entry
//> table-get
bool tableGet(Table* table, Value key, Value* value) {
  if (table->count == 0) return false;

  Entry* entry = findEntry(table->entries, table->capacity, key);
  if (!entry->occupied) return false;

  *value = entry->value;
  return true;
}
//< table-get
//> table-adjust-capacity
static void adjustCapacity(Table* table, int capacity) {
  Entry* entries = ALLOCATE(Entry, capacity);
  for (int i = 0; i < capacity; i++) {
    entries[i].occupied = false;
    entries[i].tombstone = false;
    entries[i].value = NIL_VAL;
  }
//> re-hash

//> resize-init-count
  table->count = 0;
//< resize-init-count
  for (int i = 0; i < table->capacity; i++) {
    Entry* entry = &table->entries[i];
    if (!entry->occupied) continue;

    Entry* dest = findEntry(entries, capacity, entry->key);
    dest->key = entry->key;
    dest->value = entry->value;
    dest->occupied = true;
    dest->tombstone = false;
//> resize-increment-count
    table->count++;
//< resize-increment-count
  }
//< re-hash

//> Hash Tables free-old-array
  FREE_ARRAY(Entry, table->entries, table->capacity);
//< Hash Tables free-old-array
  table->entries = entries;
  table->capacity = capacity;
}
//< table-adjust-capacity
//> table-set
bool tableSet(Table* table, Value key, Value value) {
//> table-set-grow
  if (table->count + 1 > table->capacity * TABLE_MAX_LOAD) {
    int capacity = GROW_CAPACITY(table->capacity);
    adjustCapacity(table, capacity);
  }

//< table-set-grow
  Entry* entry = findEntry(table->entries, table->capacity, key);
  bool isNewKey = !entry->occupied;
/* Hash Tables table-set < Hash Tables set-increment-count
  if (isNewKey) table->count++;
*/
//> set-increment-count
  if (isNewKey && !entry->tombstone) table->count++;
//< set-increment-count

  entry->key = key;
  entry->value = value;
  entry->occupied = true;
  entry->tombstone = false;
  return isNewKey;
}
//< table-set
//> table-delete
bool tableDelete(Table* table, Value key) {
  if (table->count == 0) return false;

  // Find the entry.
  Entry* entry = findEntry(table->entries, table->capacity, key);
  if (!entry->occupied) return false;

  // Place a tombstone in the entry.
  entry->occupied = false;
  entry->tombstone = true;
  return true;
}
//< table-delete
//> table-add-all
void tableAddAll(Table* from, Table* to) {
  for (int i = 0; i < from->capacity; i++) {
    Entry* entry = &from->entries[i];
    if (entry->occupied) {
      tableSet(to, entry->key, entry->value);
    }
  }
}
//< table-add-all
//> table-find-string
ObjString* tableFindString(Table* table, const char* chars,
                           int length, uint32_t hash) {
  if (table->count == 0) return NULL;

/* Hash Tables table-find-string < Optimization find-string-index
  uint32_t index = hash % table->capacity;
*/
//> Optimization find-string-index
  uint32_t index = hash & (table->capacity - 1);
//< Optimization find-string-index
  for (;;) {
    Entry* entry = &table->entries[index];
    if (!entry->occupied) {
      // Stop if we find an empty non-tombstone entry.
      if (!entry->tombstone) return NULL;
    } else if (IS_STRING(entry->key) &&
        AS_STRING(entry->key)->length == length &&
        AS_STRING(entry->key)->hash == hash &&
        memcmp(AS_STRING(entry->key)->chars, chars, length) == 0) {
      // We found it.
      return AS_STRING(entry->key);
    }

/* Hash Tables table-find-string < Optimization find-string-next
    index = (index + 1) % table->capacity;
*/
//> Optimization find-string-next
    index = (index + 1) & (table->capacity - 1);
//< Optimization find-string-next
  }
}
//< table-find-string
//> Garbage Collection table-remove-white
void tableRemoveWhite(Table* table) {
  for (int i = 0; i < table->capacity; i++) {
    Entry* entry = &table->entries[i];
    if (entry->occupied && IS_STRING(entry->key) &&
        !AS_STRING(entry->key)->obj.isMarked) {
      tableDelete(table, entry->key);
    }
  }
}
//< Garbage Collection table-remove-white
//> Garbage Collection mark-table
void markTable(Table* table) {
  for (int i = 0; i < table->capacity; i++) {
    Entry* entry = &table->entries[i];
    if (entry->occupied) {
      markValue(entry->key);
      markValue(entry->value);
    }
  }
}
//< Garbage Collection mark-table
