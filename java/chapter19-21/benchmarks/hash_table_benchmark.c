#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "object.h"
#include "table.h"
#include "value.h"
#include "vm.h"

#define KEY_COUNT 10000
#define LOOKUP_ROUNDS 100
#define CHURN_ROUNDS 20

static double nowSeconds(void) {
  return (double)clock() / CLOCKS_PER_SEC;
}

static void require(bool condition, const char* message) {
  if (!condition) {
    fprintf(stderr, "benchmark failure: %s\n", message);
    exit(1);
  }
}

static void fillNumbers(Table* table, int count) {
  for (int i = 0; i < count; i++) {
    tableSet(table, NUMBER_VAL((double)i), NUMBER_VAL((double)i));
  }
}

static double benchmarkInsert(void) {
  Table table;
  initTable(&table);

  double start = nowSeconds();
  fillNumbers(&table, KEY_COUNT);
  double elapsed = nowSeconds() - start;

  require(table.count == KEY_COUNT, "insert count");
  freeTable(&table);
  return elapsed;
}

static double benchmarkHits(void) {
  Table table;
  initTable(&table);
  fillNumbers(&table, KEY_COUNT);

  double start = nowSeconds();
  for (int round = 0; round < LOOKUP_ROUNDS; round++) {
    for (int i = 0; i < KEY_COUNT; i++) {
      Value value;
      require(tableGet(&table, NUMBER_VAL((double)i), &value),
              "successful lookup");
    }
  }
  double elapsed = nowSeconds() - start;

  freeTable(&table);
  return elapsed;
}

static double benchmarkMisses(void) {
  Table table;
  initTable(&table);
  fillNumbers(&table, KEY_COUNT);

  double start = nowSeconds();
  for (int round = 0; round < LOOKUP_ROUNDS; round++) {
    for (int i = KEY_COUNT; i < KEY_COUNT * 2; i++) {
      Value value;
      require(!tableGet(&table, NUMBER_VAL((double)i), &value),
              "missing lookup");
    }
  }
  double elapsed = nowSeconds() - start;

  freeTable(&table);
  return elapsed;
}

static double benchmarkDeletionChurn(void) {
  Table table;
  initTable(&table);
  fillNumbers(&table, KEY_COUNT);

  double start = nowSeconds();
  for (int round = 0; round < CHURN_ROUNDS; round++) {
    for (int i = 0; i < KEY_COUNT; i += 2) {
      require(tableDelete(&table, NUMBER_VAL((double)i)),
              "delete existing key");
    }
    for (int i = 0; i < KEY_COUNT; i += 2) {
      tableSet(&table, NUMBER_VAL((double)i), NUMBER_VAL((double)i));
    }
  }
  double elapsed = nowSeconds() - start;

  require(table.count == KEY_COUNT, "churn count");
  freeTable(&table);
  return elapsed;
}

static double benchmarkStrings(void) {
  Table table;
  initTable(&table);
  Value* keys = malloc(sizeof(Value) * KEY_COUNT);
  require(keys != NULL, "string key allocation");

  for (int i = 0; i < KEY_COUNT; i++) {
    char buffer[32];
    int length = snprintf(buffer, sizeof(buffer), "key-%d", i);
    char* chars = malloc((size_t)length + 1);
    require(chars != NULL, "string allocation");
    memcpy(chars, buffer, (size_t)length + 1);
    keys[i] = OBJ_VAL(makeString(true, chars, length));
    // Keep benchmark keys alive while stress-GC is enabled. The VM's string
    // intern table is deliberately weak, so it is not itself a root.
    tableSet(&vm.globals, keys[i], NIL_VAL);
  }

  double start = nowSeconds();
  for (int i = 0; i < KEY_COUNT; i++) {
    tableSet(&table, keys[i], NUMBER_VAL((double)i));
  }
  for (int round = 0; round < LOOKUP_ROUNDS; round++) {
    for (int i = 0; i < KEY_COUNT; i++) {
      Value value;
      require(tableGet(&table, keys[i], &value), "string lookup");
    }
  }
  double elapsed = nowSeconds() - start;

  free(keys);
  freeTable(&table);
  return elapsed;
}

static void report(const char* name, double seconds) {
  printf("%-22s %8.3f ms\n", name, seconds * 1000.0);
}

int main(void) {
  initVM();

  printf("keys=%d, lookup rounds=%d, churn rounds=%d\n",
         KEY_COUNT, LOOKUP_ROUNDS, CHURN_ROUNDS);
  report("numeric insert", benchmarkInsert());
  report("numeric hits", benchmarkHits());
  report("numeric misses", benchmarkMisses());
  report("deletion churn", benchmarkDeletionChurn());
  report("string insert/lookups", benchmarkStrings());

  freeVM();
  return 0;
}
