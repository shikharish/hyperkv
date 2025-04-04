#ifndef TEST_H
#define TEST_H

#include "../src/common.h"
#include "../src/log.h"
#include "../src/log_init.h"

void test_htable(void);
void test_parser(void);
// interpreter test
void test_interpret(void);
void cleanup(HashTable *ht);
bool compare(HashTable *ht, char *cmd, char *expected);
void test_interpret_key(HashTable *ht);
void test_interpret_str(HashTable *ht);
void test_interpret_hash(HashTable *ht);
void test_interpret_list(HashTable *ht);
void test_interpret_set(HashTable *ht);

#endif
