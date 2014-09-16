#include <stdlib.h>
#include <gtk/gtk.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include <windows.h>

#include "heap.h"
#include "list.h"

#define MAX_SIZE 1024

char *pass_word_name = "\\user\\pass_word.txt";

char *docfilename;

typedef struct Word {
	unsigned char *string;
	int leng;
	Lists *linenum;
} Word;

char current_absolute_path[MAX_SIZE];
char current_directory[MAX_SIZE];
char pass_word_absolute_name[MAX_SIZE];

FILE *output;
Root *root;
Root *passroot;

unsigned word_num = 0, word_pass = 0;
unsigned char *wordp;
Word *word;

unsigned password_num = 0, password_pass = 0;
unsigned char *passwordp;
Word *password;



void destroy_word (Word *word) {
	list_destroy (&word->linenum);
	//list_destroy_top (&word->linenum);
	free (word->string);
	free (word);
}

int cmp_string (void *data1, void *data2) {
	return stricmp(((Word*)data1)->string,((Word*)data2)->string);
}

int cmp_string1 (void *data1, void *data2) {

	unsigned char t = 'a'^'A';
	unsigned char *ap = ((Word*) data1) -> string;
	unsigned char *bp = ((Word*) data2) -> string;
	while ((*ap|t) == (*bp|t) && *ap && *bp) ap++, bp++;
	return (*ap|t) - (*bp|t);
}

int cmp_ok (void *wdata, void *data) {
	if (passroot) {
		Heap *myheap = heap_find(passroot, (void*)data);
		if (myheap) {
			(Lists*)(((Word*)wdata)->linenum)->num ++;
			//(List*)((Word*)(myheap->data)->linenum)->num++;
			destroy_word (data);
			return 0;
		}
	}

	list_add1(((Word*)wdata)->linenum,list_top(((Word*)data)->linenum));
	destroy_word ((Word*)data);
	return 0;
}

void print_linenum (void *data) {
	fprintf (output, "%d.",(int)data);
}

void print_word (void *data) {
	Word *word = (Word*) data;
	fprintf (output, "%s ", word -> string);

	int i,space;//
	space = 15 - word -> leng;
	for (i=0; i<space; i++)
		fprintf (output, "-");
	//if (word->linenum->num == 1)
	//	fprintf (output, "[  ] ");
	//else if (word->linenum->num > 99)
	//	fprintf (output, "[**] ");
	//else
	fprintf (output, "<%d> ", word->linenum->num);

	list_print (word->linenum, print_linenum);
	fprintf (output, "\n");
	//printf ("*************** ok %d\n", __LINE__); //--------->
}

Word* init_word (unsigned char *str, unsigned leng, unsigned linenum) {
	Word *word = malloc (sizeof (Word));
	word -> string = malloc (leng + 1);
	strncpy ((char*)word->string, (char*)str, leng);
	word -> string [leng] = '\0';
	word -> leng = leng;
	word -> linenum = list_init ();
	list_add1 (word->linenum, (void*)(linenum));
	return word;
}

unsigned add_to_heap (unsigned char *thebuffer, unsigned thelength, Root *root, Root *thepass) {
	unsigned i,leng, linenum;
	unsigned word_num = 0;
	unsigned char *wordp;
	Word *word;

	for (i=0,linenum=1; i<thelength; i++) {
		if (thebuffer[i] == '\n') {
			linenum++;
			continue;
		}
		if (!isalpha (thebuffer[i])) continue;
		wordp = (thebuffer + i);
		for (leng=0; isalpha (thebuffer[i]) && i<thelength; i++,leng++);
		if (leng < 3) {
			continue;        //too short
		}

		word = init_word (wordp, leng, linenum);
//		if (thepass){
//
//                        if (heap_find(thepass, (void*)word)) {
//                                destroy_word (word);
//                                continue;
//                        }
//		}
		word_num++;
		if (word == NULL)
			printf ("heap insert error!\n");
		heap_insert (root, word);
	}
	return word_num;
}

unsigned get_file_size (char *the_name) {
	struct stat sbuf;
	stat (the_name, &sbuf);
	int fd = open (the_name, O_RDONLY | O_BINARY);
	unsigned size = sbuf.st_size;
	close (fd);
	return size;
}

void save_to_file (char *the_name) {
	output = fopen (the_name, "w");
	if (output == NULL) {
		exit (-2);
	}

	fprintf (output, "");
	fprintf (output,"+++++++++++++++++++++++++++++++++++++++++++++++++++++\n");
	fprintf (output,"print string, and time, and linenum which once found!\n");
	fprintf (output,"doc-name: %s\n", docfilename);
	g_free (docfilename);
	fprintf (output,"file-name: %s\n", the_name);
	fprintf (output,"word sum: %d\n", word_num);
	//fprintf (output,"word_pass %d\n", word_pass);
	fprintf (output,"find %d\n", heap_num (root));
	fprintf (output,"+++++++++++++++++++++++++++++++++++++++++++++++++++++\n");
	heap_print (root);
	fprintf (output,"+++++++++++++++++++++++++++++++++++++++++++++++++++++\n");
	fclose (output);

}

//
char* get_current_absolute_path (char *the_path) {
	int path_leng = GetModuleFileName (NULL, the_path, MAX_SIZE);
	if ( path_leng == 0 ) {
		printf ("GetModuleFileName error!\n");
		exit(-1);
	}
	for (int i=path_leng-1; i> 0; i--) {
		if (the_path[i] == '\\') {
			the_path[i] = '\0';
			return the_path;
		}
	}
	return NULL;
}

//
void open_and_save_file (GtkWidget *win, char *the_name) {
	unsigned length = get_file_size(the_name);
	unsigned char *buffer = malloc (length);
	int fd = open (the_name, O_RDONLY | O_BINARY);
	if (fd <= 0) {
                printf ("error! can't open input file!\n");
                printf ("input file: %s\n", the_name);

        }
	read (fd, buffer, length);



	unsigned passlength = get_file_size(pass_word_absolute_name);
	unsigned char *passbuffer = malloc (passlength);
	int passfd = open (pass_word_absolute_name, O_RDONLY | O_BINARY);

	root = heap_init ();


	heap_init_func (root, cmp_string1, cmp_ok, print_word);

	passroot = heap_init ();
	heap_init_func (passroot, cmp_string1, cmp_ok, NULL);

	if (passfd > 0) {
		read (passfd, passbuffer, passlength);
		password_num = add_to_heap(passbuffer, passlength, passroot, NULL);
		word_num = add_to_heap(buffer, length, root, passroot);
	} else {
		printf ("error! can't open pass_world.txt\n");
		word_num = add_to_heap(buffer, length, root, NULL);
	}

	close (fd);
	free(buffer);
	GtkWidget *dialog;

	dialog = gtk_file_chooser_dialog_new ("Save File",
	                                      GTK_WINDOW (win),
	                                      GTK_FILE_CHOOSER_ACTION_SAVE,
	                                      GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
	                                      GTK_STOCK_SAVE, GTK_RESPONSE_ACCEPT,
	                                      NULL);
	gtk_file_chooser_set_do_overwrite_confirmation (GTK_FILE_CHOOSER (dialog), TRUE);

	gtk_file_chooser_set_current_name (GTK_FILE_CHOOSER (dialog), "Untitled document");

	if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_ACCEPT) {
		char *filename;


		filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialog));
		gtk_widget_destroy (dialog);
		save_to_file (filename);
		g_free(filename);
	} else {
		gtk_widget_destroy (dialog);
	}



}

//
static void about (GtkWidget *wid, GtkWidget *win) {
	GtkWidget *dialog = NULL;
	dialog = gtk_message_dialog_new (GTK_WINDOW (win), GTK_DIALOG_MODAL, GTK_MESSAGE_INFO, GTK_BUTTONS_CLOSE, "用于生成一个英文文档的每个单词的频率以及每次出现的行号\nauthor:lisper.li@dfrobot.com");
	gtk_window_set_position (GTK_WINDOW (dialog), GTK_WIN_POS_CENTER);
	gtk_dialog_run (GTK_DIALOG (dialog));
	gtk_widget_destroy (dialog);
}

//
static void openfunc (GtkWidget *wid, GtkWidget *win) {
	GtkWidget *dialog = NULL;

	dialog = gtk_file_chooser_dialog_new ("Open File",
	                                      GTK_WINDOW (win),
	                                      GTK_FILE_CHOOSER_ACTION_OPEN,
	                                      GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
	                                      GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
	                                      NULL);

	if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_ACCEPT) {


		docfilename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialog));
		gtk_widget_destroy (dialog);
		open_and_save_file (win, docfilename);

	} else {
		g_free (docfilename);
		gtk_widget_destroy (dialog);
	}



}

//
int main (int argc, char *argv[]) {

	get_current_absolute_path (current_absolute_path);
	sprintf (pass_word_absolute_name, "%s%s", current_absolute_path, pass_word_name);
	//strcat (current_absolute_path, pass_word_name);
	printf ("current = %s", pass_word_absolute_name);

	GtkWidget *button = NULL;
	GtkWidget *win = NULL;
	GtkWidget *vbox = NULL;

	/* Initialize GTK+ */
	//g_log_set_handler ("Gtk", G_LOG_LEVEL_WARNING, (GLogFunc) gtk_false, NULL);
	gtk_init (&argc, &argv);
	//g_log_set_handler ("Gtk", G_LOG_LEVEL_WARNING, g_log_default_handler, NULL);

	/* Create the main window */
	win = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	gtk_window_set_default_size (GTK_WINDOW (win), 180, 100);
	gtk_window_set_opacity (GTK_WINDOW (win), 0.9);
	gtk_container_set_border_width (GTK_CONTAINER (win), 5);
	gtk_window_set_title (GTK_WINDOW (win), "wordlist");
	gtk_window_set_position (GTK_WINDOW (win), GTK_WIN_POS_CENTER);
	gtk_widget_realize (win);
	g_signal_connect (win, "destroy", gtk_main_quit, NULL);

	/* Create a vertical box with buttons */
	//vbox = gtk_vbox_new (TRUE, 6);
	vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);

	gtk_container_add (GTK_CONTAINER (win), vbox);

	button = gtk_button_new_with_label("open file");
	//button = gtk_button_new_from_stock (GTK_STOCK_DIALOG_INFO);
	g_signal_connect (button, "clicked", G_CALLBACK (openfunc), (gpointer) win);
	gtk_box_pack_start (GTK_BOX (vbox), button, TRUE, TRUE, 0);

	button = gtk_button_new_with_label("about");
	g_signal_connect (button, "clicked", G_CALLBACK (about), NULL);
	gtk_box_pack_start (GTK_BOX (vbox), button, TRUE, TRUE, 0);

	button = gtk_button_new_with_label("exit");
	g_signal_connect (button, "clicked", gtk_main_quit, NULL);
	gtk_box_pack_start (GTK_BOX (vbox), button, TRUE, TRUE, 0);

	/* Enter the main loop */
	gtk_widget_show_all (win);
	gtk_main ();
	return 0;
}

