/* line.c -> data-structures holding important infos */

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
} ln;

int calcdiff(int row_a, ln* self);


ln* new_ln(ec* energycounter){
    ln* obj = (ln*)malloc(sizeof(ln));
    if(energycounter)   obj->ecp = energycounter;
    obj->calc_diff = &calcdiff;
    return obj;
}

void del_ln(ln* obj){
    if(obj->ecp)    del_ec(obj->ecp);
    if(obj)         free(obj);
}


void skipln(FILE* fp){
    char ch='0';
    while(ch!='\n'){ //skip line
        ch=fgetc(fp);
        if(ch==EOF) break;
    }
}

int calcdiff(int row_a, ln* self){
    fpos_t row_below,fp_reset;
    fgetpos(self->fp, &fp_reset);

    //getting the numbers from row_below
    skipln(self->fp);
    char ch='0';

    // going to 2nd ';' where the energy values start
    for(int i=0;i<2;i++){
        ch='0';
		//maybe use:	strchr(self->line,';');
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
    for(unsigned long j=0;j<=i;j++) energy_value[j]=fgetc(self->fp);

    //atoi row_below
    unsigned long row_b=strtoul(energy_value,NULL,10);
    free(energy_value);
    fsetpos(self->fp,&fp_reset);
    self->diff = row_b-row_a;
    return 0;
}
