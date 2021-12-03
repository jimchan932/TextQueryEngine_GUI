#ifndef PROCESSTEXT
#define PROCESSTEXT

char *queryResultParagraph;


int  paragraphByteSize;
int queryResultByteSize;
typedef struct Text Text;
struct Text
{
	char *paragraph;
	int paragraph_len;
	char sentences[1000][400];
	int num_of_sentences;
	int num_of_words;

};
typedef struct QueryWords QueryWords;
struct QueryWords
{
	char words[10][30];
	int num_of_queries;
};

char *replaceSpace(char *paragraph);

char  *processText(char *filename, int *byteSize);

void splitSentences(Text *SampleText);

void deleteTextSample(Text *SampleText);
int countWords(char *paragraph, int len);

void splitQueryWords(QueryWords *qText, char *paragraph);


void deleteQueryResult();
#endif
