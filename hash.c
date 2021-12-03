#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "hash.h"


void createItem(Item *item, ub4 key, ub1 * word, int sentence_num)
{
	item->key = key;
	strcpy(item->word,  word);
	item->frequency = 0;
	item->sentence_location[item->frequency++] = sentence_num;
}


// the length of the hash table is based on 2^n
void initHashTable(HashTable *ht, int num_of_words)
{
	ht->collisions = 0;
	ht->D = 5;

	while(loadfactor(num_of_words, ht->D) > OPTIMAL_LOAD_FACTOR)
	{
		ht->D++;
	}
	int hashSize = hashsize(ht->D);
	ht->hashTB = (Item *)malloc(sizeof(Item) * hashSize);
	ht->empty = (bool *)malloc(sizeof(bool) * hashSize);
	for(int i = 0; i < hashSize; i++)
	{
		ht->empty[i] = true;
	}	
}

void deleteHashTable(HashTable *ht)
{
	free(ht->hashTB);
	free(ht->empty);		 	
}


int hFindIndex(HashTable *ht, ub4 key, ub1 *word)
{
	int index = (key & hashmask(ht->D));
	int j = index;

	do
	{
		if(ht->empty[j] || strcmp(ht->hashTB[j].word,word) == 0)
		{
			return j;
		}		
		j = ((j+1) & hashmask(ht->D));
	} while(j != index);    
	return j; // hash table is full
}

Item *hFind(HashTable *ht, ub4 (*hashFunction)(ub1 *), ub1 *word)
{
    char findWord[30] = {0};
	strcpy(findWord, word);
	findWord[0] = tolower(findWord[0]);
	int key = hashFunction(findWord);
	int b = hFindIndex(ht, key, findWord);
	if(ht->empty[b] || strcmp(ht->hashTB[b].word, findWord) != 0) return NULL;
	return &ht->hashTB[b];
}

bool hInsert(HashTable *ht, ub4 key, ub1 *word, int sentence_num)
{
	int b = hFindIndex(ht, key, word);

	if(ht->empty[b])
	{
		// empty slot for insertion.
		createItem(&ht->hashTB[b], key, word, sentence_num);
		ht->empty[b] = false;
		return true;
	}
	else if(!ht->empty[b] && strcmp(word, ht->hashTB[b].word) == 0)
	{
		// same word, record the sentence number and frequency
		ht->hashTB[b].sentence_location[ht->hashTB[b].frequency++] = sentence_num;
		return true;
	}
	// table is full
	return false;
}

void storeWords(HashTable *hTable,			
				ub4 (*hashFunction)(ub1 *),
				ub1 *paragraph, int paragraph_len)
{
	bool process_word_flag = false;
	int sentence_num = 0;
	int src = 0;
	for(int i = 0; i < paragraph_len; i++)
	{

		bool skipFlag = paragraph[i] == ' ' || paragraph[i] == ',' || paragraph[i] == '\n' ||
			paragraph[i] == '(' || paragraph[i] == '\"' || paragraph[i] == '\'' || paragraph[i] == ')';
		if(!process_word_flag)
		{
			if(paragraph[i] == '!' || paragraph[i] == '.' || paragraph[i] == '?')
			{
				sentence_num++;
			}
			// else if(begin_closed_paran_flag)
			else if(skipFlag)
			{
				continue;
			}					
			else
			{
				process_word_flag = true;				
				src = i;
			}			   				
		}
		else
		{
			char word[30] = {0};			
			if(paragraph[i] == '!' || paragraph[i] == '.' || paragraph[i] == '?')
			{
				if(strcmp(word, "Mr") == 0 || strcmp(word, "Dr") == 0 || strcmp(word, "Ms") == 0)
				{
					
					strncpy(word, paragraph+src, i-src+1); // Dr. Mr. Ms.
					word[0] = tolower(word[0]);
			        unsigned long key = hashFunction(word);		
			  	    hInsert(hTable, key, word, sentence_num);				
					process_word_flag = false;
				}
				else
				{
					strncpy(word, paragraph + src, i-src);  	
					word[0] = tolower(word[0]);
					unsigned long key = hashFunction(word);		
					hInsert(hTable, key, word, sentence_num);				
					process_word_flag = false;
					sentence_num++; // increment the sentence number				
				}
			}
			else if(skipFlag)
			{
				
				strncpy(word, paragraph + src, i-src);
				word[0] = tolower(word[0]);
				unsigned long key = hashFunction(word);		
				hInsert(hTable, key, word, sentence_num);			
				process_word_flag = false;
			}
			else continue;
		}
	}
}

void displayItem(Item *item, char sentences[1000][400])
{
	printf("Query word: %s, Key: %lu\n", item->word, item->key); 
	printf("Query result:\n");
	for(int i = 0; i < item->frequency; i++)
	{
		printf("Sentence %d: %s\n", i+1, sentences[item->sentence_location[i]]);
	}
}

// print query result into file
void printResult(HashTable *ht,ub4 (*hashFunction)(ub1 *),char qw[10][30], int n,
				 char sentences[1000][400])
{
	FILE *fp = fopen("result.txt", "w");
	fprintf(fp, "Query result:\n");	
	for(int i = 0; i < n; i++)
	{
		
		fprintf(fp, "Query word: %s\n", qw[i]);
		
		Item *item = hFind(ht, hashFunction,  qw[i]);
		if(item != NULL)
		{
			for(int j = 0; j < item->frequency; j++)
			{		
				fprintf(fp, "Sentence %d: %s\n",
						j+1, sentences[item->sentence_location[j]]);
			}			
		}
		else
		{		
			fprintf(fp, "Word not found.\n");
		
		}
	}
	fclose(fp);
}

ub4 hfunc(ub1 *str)
{
	ub4 hash = 0;
	int c;

	while (c = *str++)
		hash = c + (hash << 6) + (hash << 16) - hash;

	return hash;
}
