/*
 * Tmsort.c
 * Created on: 29.06.2018
 *		 Author: Phahou
 */

typedef struct Thm_t {
	unsigned short int* lineno_;//thread safe (Read-only)
	char* line;					//needs to be initialised
	ln* ln_0;					//everything related to a line
	unsigned int i;				//for iteration
	fpos_t* pos;				//for setting fp on the right line in the beginning
	char filename[200];
}Tmst;

Tmst* new_Threadedmsort_argv_t(
unsigned short int* lineno_,unsigned short int* highest,const char* filename,FILE *tmp_,
unsigned int i_, fpos_t* pos_) {
	Tmst* obj=(Tmst*)malloc(sizeof(Tmst));
	obj->line=(char*)malloc(*highest*sizeof(char));
	obj->lineno_=lineno_;
	obj->ln_0=new_ln(new_ec(NULL));
	obj->ln_0->fp=fopen(filename,"r");
	obj->pos=pos_;
	obj->ln_0->ecp->tmp=tmp_;
	obj->i=i_;
	strcpy(obj->filename,filename);
	return obj;
}

void del_Threadedmsort_argv_t(Tmst* obj){
	if(obj){
		if(obj->line){
			free(obj->line);
		}
		if(obj->ln_0->fp){ //closes FILE* if open
			fclose(obj->ln_0->fp);
		}
		del_ln(obj->ln_0); //deletes ecp too
	}
	free(obj);
}
