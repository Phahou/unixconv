/*
 * Tmsort.c
 * Created on: 29.06.2018
 *       Author: Phahou
 */
#include "line.c"

typedef struct Thm_t {
    FILE* fp;                       //origin file (read only)
    FILE* tmp;                      //tmpfile for a id
    char* line;                     //needs to be initialised
        unsigned int i;             //for iteration
        const char* filename;       //TODO: needs to be replaced with sth out of list.c
        const char* id;             // ids read from the settings file
        size_t highest;
        size_t firstline_len;
}Tmst;

Tmst* new_Tmsort(
unsigned short int len_1st_line,
unsigned short int* highest,
const char* filename,
FILE *tmp_,
unsigned int i_,
const char* id_) {
    Tmst* obj=(Tmst*)calloc(1,sizeof(Tmst));
    obj->line=(char*)calloc(*highest,sizeof(char));
    obj->fp=fopen(filename,"r");
    obj->tmp=tmp_;
    obj->i=i_;
    obj->id=id_;
    obj->filename=filename;
    obj->highest=*highest;
    obj->firstline_len=len_1st_line;
    return obj;
}

void del_Threadedmsort_argv_t(Tmst* obj){
    if(obj){
        if(obj->line)   free(obj->line);
        if(obj->fp) fclose(obj->fp); // closes FILE* if open
        //if(obj->tmp)  fclose(obj->tmp);
    }
    free(obj);
}
