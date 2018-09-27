/* line.c */

/* Includes */
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include "ec.c"
#include <string.h>

typedef struct ln_t {
	FILE *fp;
	ec* ecp;
	unsigned long diff;
	char* line;

	int (*calc_diff)(int row_a,struct ln_t *self);
	void (*skipln)(struct ln_t *self);
	int	(*getlnlen)(struct ln_t *self);
	void (*getln)(struct ln_t *self);
} ln;

int calcdiff(int row_a, ln* self);
void skipline(ln *self);
int getlinelength(ln *self);
void _getline(ln *self);

ln* new_ln(ec* energycounter){
	ln* obj = (ln*)malloc(sizeof(ln));
	if(energycounter){
		obj->ecp = energycounter;
	}
	obj->calc_diff = &calcdiff;
	obj->skipln = &skipline;
	obj->getlnlen= &getlinelength;
	obj->getln=&_getline;
	return obj;
}

void del_ln(ln* obj){
	if(obj->ecp){
		del_ec(obj->ecp);
	}
	if(obj){
	free(obj);
	}
}

void skipline(ln *self){
	char ch='0';
	while(ch!='\n'){ //skip line
		ch=fgetc(self->fp);
		if(ch==EOF){
		//	perror("EOF ERROR");
			break;
		}
	}
}

int getlinelength(ln *self){
	//TODO make unsigned int
	int i=0;
	fpos_t reset;
	fgetpos(self->fp,&reset);
	while (fgetc(self->fp)!='\n') i++;
	fsetpos(self->fp,&reset);
	return i;
}


//newstuff
char* rmdoublequotes_rec(unsigned int len,char* str){
	if(str[len-1]=='"'){
	str[len-1]='\0';
	rmdoublequotes_rec(len-1,str);
	}
	if(str[0]=='"') return rmdoublequotes_rec(len,++str); //lol this works
	else return str;
}

char* rmdoublequotes(char* token){
	unsigned int len=strlen(token);
	char* str=(char*)malloc(len*sizeof(char));
	char* free_str=str;

	//work with str
	strcpy(str,token);
	str=rmdoublequotes_rec(len,str);

	//prep return
	char* mystr=(char*)malloc(strlen(str)*sizeof(char));
	strcpy(mystr,str);

	//clean up and return
	free(free_str);
	return mystr;
}

void _getline(ln *self){
	//get line length
	unsigned int len=self->getlnlen(self);
	//alloc space
	self->line=(char*)calloc(len,sizeof(char));
	char* tmpstring=(char*)calloc(len,sizeof(char));
	//get line in a string
	fpos_t reset;
	fgetpos(self->fp,&reset);
	fgets(self->line,len,self->fp);
	fsetpos(self->fp,&reset);
	//tokenize
	strcpy(tmpstring,self->line);

	char* token;
	token=strtok(self->line, ";");
	strcpy(self->ecp->time,token);

	token=strtok(NULL, ";");
	strcpy(self->ecp->id, token);

	token=strtok(NULL,";");
	self->ecp->value=atol(rmdoublequotes(token));

	strcpy(self->line,tmpstring);
	free(tmpstring);
}
//newstuff


int calcdiff(int row_a, ln* self){
	fpos_t row_below,fp_reset;
	fgetpos(self->fp, &fp_reset);

	//getting the numbers from row_below
	skipline(self);
	char ch='0';

	// going to 2nd ';' where the energy values start
	for(int i=0;i<2;i++){
		ch='0';
		while(ch!=';'){
			ch=fgetc(self->fp);
			if(ch=='=') {
				fsetpos(self->fp,&fp_reset);
				return -1; //next ID is here
			}
			if(ch==EOF) return -10;
		}
	}
	fgetc(self->fp);

	fgetpos(self->fp,&row_below);

	//Copying Energy Values to string
	unsigned long i=0;
	for(i=0;ch!='"';i++){
		ch=fgetc(self->fp);
		if(ch==EOF) return -10;
	}
	char* energy_value=(char*)calloc(sizeof(char),i+1);
	fsetpos(self->fp,&row_below);
	for(unsigned long j=0;j<=i;j++){
		energy_value[j]=fgetc(self->fp);
	}

	//atoi row_below
	unsigned long row_b=strtoul(energy_value,NULL,10);
	free(energy_value);
	fsetpos(self->fp,&fp_reset);
	self->diff = row_b-row_a;
	return 0;
}
