#include <gtk/gtk.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "processText.h"
#include "hash.h"

bool contains_text_flag= false; // determine if we have opened text file to search

QueryWords queryText;
Text inputParagraph;
HashTable table;

GtkWidget *window;
GtkWidget *verticalBox1, *verticalBox2;
GtkWidget *horizontalBox1, *horizontalBox2;

GtkWidget *button_paragraph_text, *button_search_query;    // text file 
GtkWidget *paragraph_file_select;   //   selection
GtkWidget *label_enter_queries, *label_paragraphInfo, *label_query_result;    // labels

GtkWidget *paragraphScrolledWindow, *paragraphTextView;      // scrolled window and text view 
GtkWidget *queryScrolledWindow, *queryTextView;
GtkWidget *queryResultScrolledWindow, *queryResultTextView;

void open_paragraph_file(GtkWidget *, gpointer); // Event handler for opening paragraph file
void showQueryResult(GtkWidget *, gpointer); // Display query results 

void textqueryInterface();
void textquerySignals();

int main(int argc, char **argv)
{
	gtk_init(&argc, &argv);
	textqueryInterface();
	textquerySignals();
	gtk_widget_show_all(window);
	gtk_main();
	deleteTextSample(&inputParagraph);
	deleteQueryResult();
	//	deleteQueryWords(&queryText);
	return 0;
}


/* In the text query graphical user interface (GUI), we will 
   design as follows:
   #####################################################################
   #  *button to selcet text*                                          #
   #                                                                   #
   #  *paragraph                     *Input entry: query words*        #
   #   text *                        *Output entry: query result*      #
   #                                                                   #
   #                                                                   #
   #####################################################################
*/
void textqueryInterface()
{
	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(window), "Text query search engine");
	gtk_window_set_default_size(GTK_WINDOW(window), 1000, 600);
	gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);

	
	verticalBox1 = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
	verticalBox2 = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
	horizontalBox1 = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);  	
	horizontalBox2 = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);  

	button_paragraph_text = gtk_button_new_with_label("Select file (paragraph)");
	button_search_query = gtk_button_new_with_label("Search query");
	label_enter_queries = gtk_label_new("Enter queries here:  ");
	label_paragraphInfo = gtk_label_new(NULL);		
	label_query_result = gtk_label_new("Query results:  ");	

	gtk_widget_set_halign(label_enter_queries, GTK_ALIGN_END);
	gtk_widget_set_halign(label_paragraphInfo, GTK_ALIGN_START);
	
	gtk_box_pack_start(GTK_BOX(horizontalBox1), button_paragraph_text,  FALSE, FALSE, 20);
	gtk_box_pack_start(GTK_BOX(horizontalBox1), label_paragraphInfo, TRUE, TRUE, 0);	
	gtk_box_pack_start(GTK_BOX(horizontalBox1), label_enter_queries, TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(verticalBox1), horizontalBox1,  FALSE, FALSE, 0);

	// Create the scrolled window for paragraph 
	paragraphScrolledWindow = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_set_size_request(paragraphScrolledWindow, 600, 200);
	// Create the scrolled window for query
	queryScrolledWindow = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_set_size_request(queryScrolledWindow, 400, 100);	
	queryResultScrolledWindow = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_set_size_request(queryResultScrolledWindow, 400, 250);		
	// create boxes
	gtk_box_pack_start(GTK_BOX(horizontalBox2), paragraphScrolledWindow, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(verticalBox2), queryScrolledWindow, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(verticalBox2), label_query_result, TRUE, TRUE, 0);	
	gtk_box_pack_start(GTK_BOX(verticalBox2), queryResultScrolledWindow, TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(verticalBox2), button_search_query, TRUE, TRUE, 0);	
	gtk_box_pack_start(GTK_BOX(horizontalBox2), verticalBox2, TRUE, TRUE, 0);	
	gtk_box_pack_start(GTK_BOX(verticalBox1), horizontalBox2,  TRUE,TRUE,0);

	// Create the text views  & attach it to the scrolled window
	paragraphTextView = gtk_text_view_new();
	queryTextView = gtk_text_view_new();
	queryResultTextView = gtk_text_view_new(); 
	
	gtk_container_add(GTK_CONTAINER(paragraphScrolledWindow), paragraphTextView);
	gtk_container_add(GTK_CONTAINER(queryScrolledWindow), queryTextView);
	gtk_container_add(GTK_CONTAINER(queryResultScrolledWindow), queryResultTextView);
	//
	gtk_container_add(GTK_CONTAINER(window), verticalBox1);	
}

void textquerySignals()
{
    g_signal_connect(G_OBJECT(button_paragraph_text), "clicked", G_CALLBACK(open_paragraph_file), NULL);
    g_signal_connect(G_OBJECT(button_search_query), "clicked", G_CALLBACK(showQueryResult), NULL);		
}
	
void open_paragraph_file(GtkWidget *widget, gpointer data)
{
	if(contains_text_flag) return;
	paragraph_file_select = gtk_file_chooser_dialog_new ("Open File",
				                             GTK_WINDOW(window),
						             GTK_FILE_CHOOSER_ACTION_OPEN,
							     GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
							     GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
							     NULL);

	if (gtk_dialog_run(GTK_DIALOG(paragraph_file_select)) == GTK_RESPONSE_ACCEPT)
	{
		char *filename;
		GtkTextBuffer *buffer;
		GtkTextIter start, end;		
		filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (paragraph_file_select));
		inputParagraph.paragraph = processText(filename, &paragraphByteSize);

		// processing text
		buffer=gtk_text_view_get_buffer(GTK_TEXT_VIEW(paragraphTextView));
		gtk_text_buffer_get_start_iter(buffer, &start);
		gtk_text_buffer_get_end_iter(buffer, &end);
		gtk_text_buffer_insert(buffer, &end,  inputParagraph.paragraph, paragraphByteSize);
		inputParagraph.paragraph_len = strlen(inputParagraph.paragraph);	
		inputParagraph.paragraph = replaceSpace(inputParagraph.paragraph);
		inputParagraph.num_of_words = countWords(inputParagraph.paragraph, inputParagraph.paragraph_len); 		
		splitSentences(&inputParagraph);
		
		// Insert words into Hash Table
		initHashTable(&table, inputParagraph.num_of_words);	
		storeWords(&table, &hfunc,
			  inputParagraph.paragraph, inputParagraph.paragraph_len);

		char paragraphInfo[45];
		snprintf(paragraphInfo, 45, "characters: %d, words: %d, sentences: %d",
				 inputParagraph.paragraph_len,
				 inputParagraph.num_of_words,
				 inputParagraph.num_of_sentences);
		
		gtk_label_set_text(GTK_LABEL(label_paragraphInfo), paragraphInfo);
		contains_text_flag = true;
		g_free (filename);
	}

	gtk_widget_destroy (paragraph_file_select);
}

void showQueryResult(GtkWidget *widget, gpointer data)
{
	if(!contains_text_flag)
	{
		gtk_label_set_text(GTK_LABEL(label_paragraphInfo), "Error: Text entry to search is NULL");
		return;		
	}
	
	// Retrieve query words from GUI
	char *filename;
	GtkTextIter start1, end1;		
	GtkTextBuffer *buffer_queries;
	gchar *queryParagraph;

	buffer_queries =gtk_text_view_get_buffer(GTK_TEXT_VIEW(queryTextView));	
	gtk_text_buffer_get_start_iter(buffer_queries, &start1);
	gtk_text_buffer_get_end_iter(buffer_queries, &end1);	
   
	queryParagraph = gtk_text_buffer_get_text(buffer_queries , &start1 , &end1 , TRUE);
	// #TODO: Add case for empty queries 
	splitQueryWords(&queryText, queryParagraph);

	// print query results to GUI
	GtkTextIter start2, end2;		
	GtkTextBuffer *buffer_query_result;
	buffer_query_result = gtk_text_view_get_buffer(GTK_TEXT_VIEW(queryResultTextView));
	gtk_text_buffer_get_start_iter(buffer_query_result, &start2);
	gtk_text_buffer_get_end_iter(buffer_query_result, &end2);


    	printResult(&table, hfunc, queryText.words, queryText.num_of_queries, inputParagraph.sentences);	
	queryResultParagraph = processText("result.txt", &queryResultByteSize);	
	
    	gtk_text_buffer_insert(buffer_query_result, &end2, queryResultParagraph, queryResultByteSize);
	contains_text_flag = true;
	g_free(queryParagraph);
	g_free (filename);	
}
