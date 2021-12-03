#ifndef HASH_H
#define HASH_H
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
typedef  unsigned long  ub4;   /* unsigned 4-byte quantities */
typedef  unsigned char ub1;   /* unsigned 1-byte quantities */

#define hashsize(n) ((ub4)1<<(n))
#define hashmask(n) (hashsize(n)-1)
#define loadfactor(i, n) (i/hashsize(n))
#define OPTIMAL_LOAD_FACTOR 0.75
typedef struct Item Item;

struct Item
{
	ub4 key;	
	char word[30];
	int sentence_location[1000];
	int frequency;   
};

typedef struct HashTable HashTable;
struct HashTable
{
	int collisions;
	int D;
	Item *hashTB;
	bool *empty;
};

void createItem(Item *item, ub4 key, ub1 *word, int sentence_num);
void initHashTable(HashTable *ht, int num_of_words);

void deleteHashTable(HashTable *ht);
int hFindIndex(HashTable *ht, ub4 key, ub1 *word);
Item *hFind(HashTable *ht, ub4 (*hashFunction)(ub1 *), ub1 *word);
bool hInsert(HashTable *ht, ub4 key, ub1 *word, int sentence_num);

void storeWords(HashTable *hTable,				
				ub4 (*hashFunction)(ub1 *),
				ub1 *paragraph, int paragraph_len);
void displayItem(Item *item, char sentences[1000][400]);
ub4 hfunc(ub1 *str);

void printResult(HashTable *ht,ub4 (*hashFunction)(ub1 *),char qw[10][30], int n,
				  char sentences[1000][400]);
#endif
