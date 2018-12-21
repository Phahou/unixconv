/* line.c -> data-structures holding important infos */

/* Includes */
#include <stdio.h>
#include <stdlib.h>
#include "ec.c"

typedef struct ln_t {
    FILE *fp;
    ec* ecp;
} ln;


ln* new_ln(ec* energycounter){
    ln* obj = (ln*)malloc(sizeof(ln));
    if(energycounter)   obj->ecp = energycounter;
    return obj;
}

void del_ln(ln* obj){
    if(obj){
        if(obj->ecp)    del_ec(obj->ecp);
        if(obj->fp)     fclose(obj->fp);
        free(obj);
    }
}