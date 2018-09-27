
#define GRN	 "\x1B[32m"
#define WHT	 "\x1B[37m"
#define RESET "\x1B[0m"
#define BOLD	"\x1B[1m"
#define DEF_FILENAME "Meter_"

#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>

/* string */
typedef struct STR_T{
	size_t size;
	char* str;
}STR;

/* list */
typedef struct list_t{
	void* str;
	struct list_t* next;
} list;

/* tree */
typedef struct tree_list{
	struct tree_t* dir;
	struct tree_list* next;
}tree_l;

typedef struct tree_t{
	STR* dir;				// own dir name
	list* files;		// files in dir
	struct tree_t* subdir; // subdirs in dir
}tree;

/* tree */
tree* new_tree(STR* dir);
void del_tree(tree* obj);
void del_complete_tree(tree* obj);
tree* t_add_subdir(tree* self, tree* subdir); //is fast if self==curr_pos
list* t_add_list(tree* self, list* newfiles);//is fast if self==curr_pos
char* construct_path(tree* self);

/* string */
STR* new_str(char* str,size_t size);
void del_str(STR* str);

/* list */
list* new_list(char* data);
list* addlast(list* new_elem, list* last_elem);
void del_list(list* obj);
void del_complete_list(list* elem);
void printall(list* head);

/* sort */
void swap(list *a, list *b);
void bubbleSort(list *start);

/* tree functions */
tree* new_tree(STR* dir){
	tree* obj=(tree*)calloc(sizeof(tree),1);
	if (dir){
		obj->dir=dir;
	}
	obj->files=NULL;
	obj->subdir=NULL;
	return obj;
}
void del_tree(tree* obj){
	if(obj->dir){
		del_str(obj->dir);
	}
	if(obj->files){
		del_complete_list(obj->files);
	}
	free(obj);
}
void del_complete_tree(tree* obj){
	tree* obj_b;
	while(obj){
		obj_b=obj;
		obj=obj->subdir;
		del_tree(obj_b);
	}
}

//returns a pointer to the newly added subdir in self
tree* t_add_subdir(tree* self, tree* subdir){
	if (self){
		if (subdir){
			self->subdir=subdir;
			return subdir;
		}
	} else if(subdir) {
		return subdir;
	}
	return self;
}
//returns a pointer to the newly added list in tree self{}{}
list* t_add_list(tree* self, list* newfiles){
	if(self){
		if(newfiles){
			self->files=newfiles;
			return newfiles;
		}
	} else if(newfiles) {
		return newfiles;
	}
	return NULL;
}

/* STR functions */
STR* new_str(char* str, size_t size){ //O(1)
	STR* obj=(STR*)calloc(sizeof(STR),1);
	obj->str=str;
	if(size>0){
		obj->size=size;
	} else {
		obj->size=strlen(str);
	}
	return obj;
}
void del_str(STR* obj){ //O(1)
	if(obj->str){
		free(obj->str);
	}
	free(obj);
}
/* list functions */
list* new_list(char* data){ //O(1)
	if(!data) {
		return NULL;
	}
	list* obj=(list*)malloc(sizeof(list));
	obj->str=data;
	obj->next=NULL;
	return obj;
}
void del_list(list* obj){ //O(1)
	if(obj->str){
		free(obj->str);
	}
	free(obj);
}
list* addlast(list* new_elem, list* last_elem){ //O(1) with checks if NULL
	if(last_elem){		//if ((last_elem!=NULL)
		if(new_elem){
			last_elem->next=new_elem;
			return new_elem;
		} else {
			return last_elem;
		}
	} else if(new_elem){
		return new_elem;
	} else {
		fprintf(stderr,"List was lost(new_elem==NULL)&(last_elem==NULL)\n");
	}
	return new_elem;
}
void del_list_last(list* head){ //O(n)
	while(true){
		if(head->next){
			head=head->next;
		} else {
			free(head->str);
			break;
		}
	}
}
void del_complete_list(list* obj){ //O(n)
	list* obj_b;
	while (obj){
		obj_b=obj;
		obj=obj->next;
		del_list(obj_b);
	}
}

/* print */
void printall(list* head){
	printf("\nList of all files in the inputed folders(recursively):\n");
	while (head){
		if (head->str){
			printf("%s\n",(char*)head->str);
		}
		head=head->next;
	}
}

/* function to swap data of two nodes a and b*/
void swap(list *a, list *b) {
	list temp = *a;
	a->str = b->str;
	b->str = temp.str;
}

/* Bubble sort the given linked list */
void bubbleSort(list *start) {
	bool swapped=true;
	list *ptr1;
	list *lptr = NULL;

	/* Checking for empty list */
	if (!start) {
		return;
	}

	while (swapped) {
		swapped = false;
		if(start->next){
			ptr1 = start->next;
		}

		while ((ptr1->next != lptr)&&(ptr1->next)) {
			if (strcmp(ptr1->str,ptr1->next->str)<0) {
				swap(ptr1, ptr1->next);
				swapped = true;
			}
			ptr1 = ptr1->next;
		}
		lptr = ptr1;
	}
}
