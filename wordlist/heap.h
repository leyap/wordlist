/*

	name:	heap.h

*/

/*
cmp_ft cmpfun;
cmpok_ft cmpokfun;
print_ft printfun;

#define CMP_FUN(x) (cmpfun = (x))
#define CMPOK_FUN(x) (cmpokfun = (x))
#define PRINT_FUN(x) (printfun = (x))

print_ft printfun = NULL;
*/

#define p_error(x) { perror(x); exit (1); }

typedef struct heap {
	struct heap *l, *r;
	void *data;	
}Heap;

typedef struct root_s {
	Heap *rootp;
	int num;
	int (*cmp_fp) (void*, void*);
	int (*cmpok_fp) (void*, void*);
	void (*print_fp) (void*);
}Root;

Root* heap_init () {
	Root *root = (Root *) malloc (sizeof (Root));
	if (root == NULL)
		p_error ("malloc error! in init_root()");
	root->rootp = NULL;
	root->num = 0;
	return root;
}

void heap_init_func (Root* root, 
		int(*cmp_fp)(void*,void*), 
		int(*cmpok_fp)(void*, void*), 
		void(*print_fp)(void*)) {
	root->cmp_fp = cmp_fp;
	root->cmpok_fp = cmpok_fp;
	root->print_fp = print_fp;
}

Heap* heap_new (void *data) {
	Heap *heap = (Heap *) malloc (sizeof (Heap));
	if (heap == NULL)
		p_error ("malloc error! in create_new()");
	heap->l = heap->r = NULL;
	heap->data = data;
	return heap;
}

int heap_insert (Root *root, void *data) {
	Heap* heap = root->rootp;
	Heap **save = &(root->rootp);
	int cmp;
	while (heap) {
		cmp = root->cmp_fp (data, heap->data);
		if (cmp > 0) {
			save = & (heap->r);
			heap = heap -> r;
		}else
		if (cmp < 0) {
			save = & (heap->l);
			heap = heap -> l;
		}else {
			//return root->cmpok_fp (data);
			return root->cmpok_fp (heap->data, data);
		}
	}
	*save = heap_new (data);
	root->num ++;
	return 0;
}


Heap* heap_find (Root *root, void *data) {
	Heap* heap = root->rootp;
	int cmp;
	while (heap) {
		cmp = root->cmp_fp (data, heap->data);
		if (cmp > 0) {
			heap = heap -> r;
		}else
		if (cmp < 0) {
			heap = heap -> l;
		}else {
			return heap;	
		}
	}
	return (Heap*)0;
}

void* heap_find1 (Root *root, void *data) {
	Heap* heap = root->rootp;
	int cmp;
	while (heap) {
		cmp = root->cmp_fp (data, heap->data);
		if (cmp > 0) {
			heap = heap -> r;
		}else
		if (cmp < 0) {
			heap = heap -> l;
		}else {
			return heap->data;
		}
	}
	return (void*)0;
}

static void print_mid (Heap* heap, void(*print_fp)(void*)) {
    if (heap) {
	print_mid (heap->l,print_fp);
    	print_fp (heap->data);
    	print_mid (heap->r,print_fp);
    }
}
void heap_print (Root *root) {
    print_mid (root->rootp, root->print_fp);
}

int heap_num (Root *root) {
	return root->num;
}

/*
//坏掉了
static void print_mid (Heap* heap) {
	if (heap) {
		print_mid (heap->l);
		print_fp (heap->data);
		print_mid (heap->r);
	}
}

//坏掉了
void heap_print (Root *root) {
	print_mid (root->rootp);
	printf ("\n");
}

*/
