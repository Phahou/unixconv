/* idsort.c: Sorting files bc BUS Systems aren't perfect */

#include"Tmsort.c"
//#include"line.c"
#ifdef __WIN32
#include<windows.h>

#define alloc_thread          \
    DWORD t_id[cfg->dn];        \
    HANDLE t_id_handle[cfg->dn];  /* windows thread id type */

#define create_thread /*262144 bytes for stack alloc ~> 1/4MB */ \
    #define t_id_handle[i]=CreateThread(NULL,262144,&msort,threads[i],0,t_id[i]);

#define close_threads \
    CloseHandle(t_id_handle[i]);

/* Data type of msort function*/
DWORD WINAPI
#endif // __WIN32

#ifdef __unix__
#include<pthread.h> /* for multithreading in idsort() */

#define alloc_thread /*unix thread id type*/ \
    pthread_t* t_id=(pthread_t*)calloc(cfg->dn,sizeof(pthread_t));

#define create_thread \
    pthread_create(&t_id[i],NULL ,msort, threads[i]);

#define close_threads \
    pthread_join(t_id[i], NULL);

void*
#endif // __unix__
 msort(void* th_);

FILE* appendFILE(FILE* dest, FILE* src);


//Custom Filenames (Getted with a call to loadcfg() )
int idsort( char* filename, int opt,
size_t* highest, struct cfg_t* cfg,size_t* offsetcid){
//tmp & fp used here
    FILE** tmp=(FILE**)calloc(cfg->dn,sizeof(FILE*));
    FILE* tmp0=NULL;  //init file for combining the sorts (algo is like merge-sort)
    size_t len_1st_line=0; //without '\n' char

{//copy first line from filename to tmp0
    FILE* fp=fopen(filename,"r");
    tmp0=fopen("tmp0.csv","w");
    char ch=fgetc(fp);
    do {
        fprintf(tmp0,"%c",ch);
        len_1st_line++;
    } while((ch=fgetc(fp))!='\n' );
    fprintf(tmp0,"\n");
    fclose(fp);
}//end local scope

    if(opt & 8) printf(BOLD WHT "Generating tmp files...\n" RESET);
//initializing tmp files
    for(size_t i=0;i<cfg->dn;i++) tmp[i]= tmpfile();

//sort ids in specific files
{ // begin local scope "multi-threaded-sort"

    if(opt & 8) printf("...... Sorting <%s>",filename);

    alloc_thread;

    Tmst** threads=(Tmst**)calloc(cfg->dn,sizeof(Tmst*)); //data passed into threads
    for(size_t i=0;i<cfg->dn;i++){
        threads[i] =new_Tmsort( len_1st_line, highest, filename, tmp[i], cfg->id[i]);
        create_thread;
    }

//needs to be here unix does this in the #define close_threads btw
    #ifdef __WIN32
        WaitForMultipleObjects(cfg->dn,t_id_handle,TRUE,INFINITE);
    #endif // __WIN32

        //join threads
    for(size_t i=0;i<cfg->dn;i++){
        close_threads;
        del_Threadedmsort_argv_t(threads[i]);   /* free everything except
                                                 * the tmpfile-pointer */
    }

    free(threads);
    free(t_id);
} //end local scope "multi-threaded-sort"

    if(opt & 8){
        printf("\r" BOLD WHT "[" GRN "done" WHT "]" RESET "\n");
        printf("...... Merging tmp files");
    }

//cp each file into tmp0.csv
{// "combine"
    for(size_t i=0;i<cfg->dn;i++){
        //freopen("tmp0.csv","a",tmp0); //maybe this can be deleted
        offsetcid[i]=ftell(tmp0); //note where the next ID begins
        fprintf(tmp0 ,"====ID:<%s>====\n",cfg->cid[i]);
        appendFILE(tmp0,tmp[i]);
        fclose(tmp[i]);
    } //begin local scope "combine"
    //merge end
    fclose(tmp0);
    free(tmp);
    if(opt & 8) printf("\r" BOLD WHT "[" GRN "done" WHT "]" RESET "\n");
}//end "combine"
    return 0;
}




//msort also reduces

#ifdef __unix__
void*
#endif // __unix__
#ifdef __WIN32
DWORD WINAPI
#endif // __WIN32
 msort(void* th_){
    Tmst* th=(Tmst*)th_;
    char* ch=NULL;
    unsigned short int lineno=1;
    size_t clear_length=th->highest-1; //to save a substraction every loop
    fseek(th->fp,th->firstline_len,SEEK_SET);

    do {
        ch=(char*)fgets(th->line, th->highest, th->fp);
    //remove newlines
        if(ch){ //no more puzzling around if I should do +1 -1 +2 or so...
            size_t len=strlen(ch);
            for(size_t i=0; i<strlen(ch);i++){
                if(ch[len-i]!='\n') ;
                else ch[len-i]='\0';
            }
        }

    //reduce when I am already at it
        if(strstr(th->line,th->id)) {
            char* time=th->line;
            char* values=strchr(th->line,';'); // -> ;
            values[0]='\0';
            values=strchr(&values[1],';'); //now we are on the values
        //cut the rest of the line
            {
            values=&values[1]; //move values one forward
            char* cut=strchr(values,';'); //set the next ; to '\0'
            cut[0]='\0';
            }
        //remove doublequotes
            {
            if(values[0]=='"' && values[1] ) values=&values[1];
            if(values[strlen(values)-1]=='"') values[strlen(values)-1]='\0';
            }
            fprintf(th->tmp,"%s;%s\n",time,values);
        }

    //clear string for next use
        //clear_length to save 1 substraction every loop (th->highest-1)
        memset(th->line,0,clear_length);
        lineno++;
    } while(ch);

    fflush(th->tmp);

    #ifdef __unix__
    pthread_exit(0);
    #endif // __unix__
    #ifdef __WIN32
    return 0;
    #endif // __WIN32
}




/*
 * appendFILE()
 * Appends src to the end of dest and flushes dest
 * returns dest
*/

FILE* appendFILE(FILE* dest, FILE* src) {
    //TODO: Use fstat function or something like for getting the file size
    fseek(src, 0, SEEK_END);
    size_t src_size = ftell(src);
    fseek(src, 0, SEEK_SET);
    char* string=(char*)calloc(src_size+1,sizeof(char));
    fread(string, src_size, 1, src);
    fprintf(dest,"%s",string);
    //memory
    fflush(dest);
    free(string);
    return dest;
}

