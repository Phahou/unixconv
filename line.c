/* line.c */

/* Includes */
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include "ec.c"

typedef struct ln_t {
  FILE *fp;
  ec* ecp;
  unsigned long diff;
  int (*calc_diff)(int row_a,struct ln_t *self);
  void (*skipln)(struct ln_t *self);
  int  (*getlnlen)(struct ln_t *self);
} ln;


int calcdiff(int row_a, ln* self);
void skipline(ln *self);
int getlinelength(ln *self);


ln* new_ln(ec* energycounter){
  ln* obj = (ln*)malloc(sizeof(ln));
  obj->ecp = energycounter;
  obj->calc_diff = &calcdiff;
  obj->skipln = &skipline;
  obj->getlnlen= &getlinelength;
  return obj;
}

void del_ln(ln* obj){
  if(obj->ecp!=NULL){
    del_ec(obj->ecp);
  }
  if(obj!=NULL){
	free(obj);
  }
}

void skipline(ln *self){
  char ch='0';
  while(ch!='\n'){ //skip line
    ch=fgetc(self->fp);
    if(ch==EOF){
      perror("EOF ERROR");
      break;
    }
  }
}

int getlinelength(ln *self){
  int i=0;
  fpos_t reset;
  fgetpos(self->fp,&reset);
  while (fgetc(self->fp)!='\n') i++;
  fsetpos(self->fp,&reset);
  return i;
}

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
  //done fp is now on "
  //let fp advance one char bc '"' - idk if fp++ would work
  //EDIT: no it wouldnt
  fgetc(self->fp);

  fgetpos(self->fp,&row_below);

  //Copying Energy Values to string
  unsigned long i=0;
  for(i=0;ch!='"';i++){
    ch=fgetc(self->fp);
    if(ch==EOF) return -10;
  }
  char* energy_value=(char*)malloc(sizeof(char)*i);
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
