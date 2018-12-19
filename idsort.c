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

int idsort( char* filename, int opt,
unsigned short int* lineno_, unsigned short int* highest,
struct cfg_t* cfg){ //Custom Filenames (Getted with a call to loadcfg() )
//tmp & fp used here
    FILE** tmp=(FILE**)calloc(cfg->dn,sizeof(FILE*));

    if(opt & 8) printf(BOLD WHT "Generating tmp files...\n" RESET);
    
//initializing tmp files
    for(size_t i=0;i<cfg->dn;i++) tmp[i]= tmpfile();

    //sort ids in specific files
    
{ // begin local scope "multi-threaded-sort"

    if(opt & 8) printf("...... Sorting <%s>",filename);
    alloc_thread;

    Tmst** threads=(Tmst**)calloc(cfg->dn,sizeof(Tmst*)); //data passed into threads
    for(size_t i=0;i<cfg->dn;i++){
        threads[i] =new_Tmsort( lineno_,highest, filename, tmp[i],i,cfg->id[i]);
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

    FILE* tmp0=NULL;  //init file for combining the sorts (algo is like merge-sort)

//cp first line
{ //begin local scope "copyfirstline"
    FILE* fp=fopen(filename,"r");
    char* line=(char*)calloc( lineno_[0]+2, sizeof(char));
    fgets(line,lineno_[0]+2,fp);

//closing filename:
    fclose(fp);

    tmp0=fopen("tmp0.csv","w");
    fprintf(tmp0,"%s",line);
    free(line);
    if(opt & 8){
        printf("\r" BOLD WHT "[" GRN "done" WHT "]" RESET "\n");
        printf("...... Merging tmp files");
    }
} //end local scope "copyfirstline"


//cp each file into tmp0.csv
// "combine"
    for(size_t i=0;i<cfg->dn;i++){
        freopen("tmp0.csv","a",tmp0); //maybe this can be deleted
        fprintf(tmp0 ,"====ID:<%s>====\n",cfg->cid[i]);
        appendFILE(tmp0,tmp[i]);
        fclose(tmp[i]);
    } //begin local scope "combine"
    //merge end
    fclose(tmp0);
    free(tmp);
//end "combine"

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
    fseek(th->fp,th->lineno_[0],SEEK_SET);

    do {// should check why dafuq I have to do th->lineno_[lineno]+2
		// I think it's bc newline + '\0' but dafuq
        ch=(char*)fgets(th->line, th->lineno_[lineno]+2, th->fp);

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
		memset(th->line,0,th->lineno_[lineno]);
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

