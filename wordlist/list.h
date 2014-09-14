/*  
 *	list.h
 *	Lists, List
 *	list_init ()
 *	list_add ()
 *	list_add1 ()
 *	list_top ()
 *	list_print ()
 *	list_destroy ()
 *	list_destroy_top ()
 */
typedef struct List {
    struct List *next;
    void *data;
}List;

typedef struct Lists {
    int num;
    List *top;
    List *end;
}Lists;

Lists* list_init () {
    Lists *list = malloc (sizeof (Lists));
    list->num = 0;
    list->top = list->end = NULL;
    return list;
}

int list_add (Lists *list, void *data) {
    if (list->num == 0) {
	list->top=list->end = malloc (sizeof (List));
	list->top->data = data;
	list->top->next = NULL;
    }
    else {
	list->end->next = (List*) malloc (sizeof (List));
    	list->end = list->end->next;
    	list->end->data = data;
    	list->end->next= NULL; //error
    }
    list->num ++;
    return 0;
}

int list_add1 (Lists *list, void *data) {
    if (list->num == 0) {
	list->top=list->end = malloc (sizeof (List));
	list->top->data = data;
	list->top->next = NULL;
    }
    else if (list->end->data != data) { // 特别加上的 
    	if (list->end->next != 0)
    	printf ("error !");
	list->end->next = (List*) malloc (sizeof (List));
    	list->end = list->end->next;
    	list->end->data = data;
    	list->end->next = NULL; //error
    }
    list->num ++;
    return 0;
}

void *list_top (Lists *list) { 
    return list->top->data;
}

void list_print (Lists* list, void (*print_fp) (void*)) {
    List *t = list->top;
    if (t)
	for (; t; t=t->next) {
	    print_fp (t->data);
	}
}

void list_destroy_top (Lists** list) {
    free ((*list)->top);
    free (*list);
    *list = NULL;
}

void list_destroy (Lists** list) {
    List* t = (*list)->top;
    List* t1;
    while (t) {
    	t1 = t->next;
	free (t);
	t = t1;
    }
    free (*list);
    *list = NULL;
}
