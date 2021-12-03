#include "processText.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>


char *replaceSpace(char *paragraph)
{
	char *current_pos = strchr(paragraph, '\n');
	while(current_pos)
	{
		*current_pos = ' ';
		current_pos = strchr(current_pos, '\n');		
	}
	return paragraph;
}

char *processText(char *filename, int *byteSize)
{
	long length;
	char *buffer;

	FILE *f = fopen(filename, "r");

	if (f)
	{
	  fseek (f, 0, SEEK_END);
	  length = ftell (f);
	  fseek (f, 0, SEEK_SET);
	  buffer = malloc (length);
	  if (buffer)
	  {
		*byteSize = fread (buffer, 1, length, f);
	  }
	  fclose (f);
	}
	return buffer;
}

// return number of sentences 
void splitSentences(Text *SampleText)
{
	SampleText->num_of_sentences = 0;
	int src = 0;
	for(int i = 0; i < SampleText->paragraph_len; i++)
	{
		if(SampleText->paragraph[i] == '!' ||
		   SampleText->paragraph[i] == '.' ||
		   SampleText->paragraph[i] == '?')
		{			
			strncpy(SampleText->sentences[SampleText->num_of_sentences++], SampleText->paragraph + src, i-src+1); 
			src = i+1;
			
		}		
	}
}
void splitQueryWords(QueryWords *qText, char *paragraph)
{
	qText->num_of_queries = 0;
	int paragraph_len = strlen(paragraph);
	int src = 0;
	for(int i = 0; i < paragraph_len; i++)
	{
		if(paragraph[i] == ' ' || paragraph[i] == ',' || paragraph[i] == '\n')		   
		{			
			strncpy(qText->words[qText->num_of_queries++], paragraph + src, i-src); 
			src = i+1;			
		}		
	}
	//if(only_one_query_flag)
	strncpy(qText->words[qText->num_of_queries++], paragraph + src, strlen(paragraph) - src);
}


void deleteTextSample(Text *SampleText)
{
	free(SampleText->paragraph);
	for(int i = 0; i < SampleText->num_of_sentences; i++)
	{
		free(SampleText->sentences[i]);
	}
}

void deleteQueryResult()
{
	free(queryResultParagraph);
}
int countWords(char *paragraph, int len)
{
	int count = 0;
	bool word_flag = false;
	for(int i = 0; i < len; i++)
	{
		bool sign_flag = paragraph[i] == ' ' || paragraph[i] == '.' ||
			            paragraph[i] == '?' || paragraph[i] == '!' ||
			            paragraph[i] == ',' == paragraph[i] == '\"';
		if(!word_flag)
		{
			if(sign_flag)
			{
				continue;
			}
			else
			{
				count++;
				word_flag = true;				
			}
		}
		else
		{
			if(sign_flag)
			{
				word_flag = false;
			}
			else continue;
		}
	}
	return count;
}

