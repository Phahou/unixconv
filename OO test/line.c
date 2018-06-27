/* line.c */

/* Includes */
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include "ec.c"

typedef struct ln_t ln;
void calcdiff(int row_a, ln* self, int opt);
void skipline(ln *self);

typedef struct ln_t {
  FILE *fp;
  ec* counter;
  unsigned long diff;
  void (*calc_diff)(int row_a,ln *self, int opt);
  void (*skipln)(ln *self);
} ln;

ln* new_ln(ec* energycounter){
  ln* obj = (ln*)malloc(sizeof(ln));
  obj->counter = energycounter;
  obj->calc_diff = &calcdiff;
  obj->skipln = &skipline;
  return obj;
}

void skipline(ln *self){
  char ch='0';
  while(ch!='\n'){ //skip line
    ch=fgetc(self->fp);
    if(ch==EOF) perror("Error: -10");
  }
}

void calcdiff(int row_a, ln* self, int opt){
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
        perror("Error: -1"); //next ID is here
      }
      if(ch==EOF) perror("Error: -10");
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
    if(ch==EOF) perror("Error: -10");
  }
  char* energy_value=(char*)malloc(sizeof(char)*i);
  fsetpos(self->fp,&row_below);
  for(unsigned long j=0;j<=i;j++){
    energy_value[j]=fgetc(self->fp);
  }
  //atoi row_below
  unsigned long row_b=atol(energy_value);
  free(energy_value);
  fsetpos(self->fp,&fp_reset);
  self->diff = row_b-row_a;
}
