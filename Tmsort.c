/*
 * Tmsort.c
 * Created on: 29.06.2018
 *       Author: Phahou
 */
#include "line.c"

typedef struct Thm_t {
    FILE* fp;                   //origin file (read only)
    FILE* tmp;                  //tmpfile for a id
    char* line; 				//needs to be initialised
    const char* id;             // ids read from the settings file
    size_t highest;				//highest size of a line
    size_t firstline_len;		//sizeof the first line
}Tmst;

Tmst* new_Tmsort(USI len_1st_line, size_t* highest, const char* filename,
FILE *tmp_, const char* id_) {
    Tmst* obj=(Tmst*)calloc(1,sizeof(Tmst));
    obj->line=(char*)calloc(*highest,sizeof(char));
    obj->fp=fopen(filename,"r");
    obj->tmp=tmp_;
    obj->id=id_;
    obj->highest=*highest;
    obj->firstline_len=len_1st_line;
    return obj;
}

void del_Threadedmsort_argv_t(Tmst* obj){
    if(obj){
        if(obj->line)   free(obj->line);
        if(obj->fp) fclose(obj->fp); // closes FILE* if open
        //if(obj->tmp)  fclose(obj->tmp); no we don't do that here
    }
    free(obj);
}
