#ifdef __unix__
#define GRN	"\x1B[32m" //colors for the terminal
#define WHT	"\x1B[37m"
#define RESET	"\x1B[0m"
#define BOLD 	"\x1B[1m"
#endif // __unix__

#ifdef __WIN32
#define GRN	"\x1B[32m" //colors for the terminal
#define WHT	"\x1B[37m"
#define RESET	"\x1B[0m"
#define BOLD	"\x1B[1m"
#endif // __WIN32

#define DEF_FILENAME "Meter_"

#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>


/* list */
typedef struct list_t{
	void* str;
	struct list_t* next;
} list;

/* list */
list* new_list(char* data,bool isstatic);
list* addlast(list* new_elem, list* last_elem);
void del_list(list* obj);
void del_complete_list(list* elem);
void printall(list* head);

/* sort */
void swap(list *a, list *b);
void bubbleSort(list *start);

/* list functions */
list* new_list(char* data,bool isstatic){ //O(1)
	if(!data) return NULL;

	list* obj=(list*)malloc(sizeof(list));

	if(isstatic){
		obj->str=(char*)calloc(sizeof(char),strlen(data)+1);
		strcpy((char*)obj->str,data);
	} else obj->str=data;
	
	obj->next=NULL;
	return obj;
}
void del_list(list* obj){ //O(1)
	if(obj->str)	free(obj->str);
	obj->next=NULL;
	free(obj);
}
list* addlast(list* new_elem, list* last_elem){ //O(1) with checks if NULL
	if(last_elem){		//if ((last_elem!=NULL)
		if(new_elem){
			last_elem->next=new_elem;
			return new_elem;
		} else 	return last_elem;
	} else if(new_elem) return new_elem;
	else fprintf(stderr,"List was lost(new_elem==NULL)&(last_elem==NULL)\n");
	return new_elem;
}
void del_list_last(list* head){ //O(n)
	while(true){
		if(head->next)	head=head->next;
		else {
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
	//printf("\nList of all files in the inputed folders(recursively):\n");
	if(!head) printf("(null)\n");
	else {
		while (head){
			if (head->str)	printf("%s\n",(char*)head->str);
			head=head->next;
		}
	}
	printf("\n");
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
	list *ptr1 = NULL;
	list *lptr = NULL;

	/* Checking for empty list */
	if ((start)&&(start->next)) ;
	else return;

	while (swapped) {
		swapped = false;
		if(start->next)	ptr1 = start->next;

		while ((ptr1->next != lptr)&&(ptr1->next)) {
			if( ((char*)ptr1->str) && ((char*)ptr1->next->str) ){
				if (strcmp((char*)ptr1->str,(char*)ptr1->next->str)<0) {
					swap(ptr1, ptr1->next);
					swapped = true;
				}
			}
			ptr1 = ptr1->next;
		}
		lptr = ptr1;
	}
	//end of function
}
