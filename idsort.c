/* idsort.c: Sorting files bc BUS Systems aren't perfect */

#include"Tmsort.c"

#ifdef __WIN32
#include<windows.h>
#endif // __WIN32

#ifdef __unix__
#include<pthread.h> /* for multithreading in idsort() */
void*
#endif // __unix__

#ifdef __WIN32
DWORD WINAPI
#endif // __WIN32
msort(void* th_);
FILE* appendFILE(FILE* dest, FILE* src);

int idsort( char* filename, int opt,
unsigned short int* lineno_, unsigned short int* highest,
struct cfg_t* cfg){ //Custom Filenames (Getted with a call to loadcfg() )
	//tmp & fp here used
	FILE** tmp=(FILE**)calloc(cfg->dn,sizeof(FILE*));

	if(opt & 8) printf(BOLD WHT "Generating tmp files...\n" RESET);
	
	//initializing tmp files
	for(int i=0;i<cfg->dn;i++) tmp[i]= tmpfile();

	//sort ids in specific files
	{
		if(opt & 8) printf("...... Sorting <%s>",filename);

		#ifdef __WIN32
		DWORD t_id[cfg->dn];          //windows thread id type
		HANDLE t_id_handle[cfg->dn];  //
		#endif // __WIN32
		#ifdef __unix__
		pthread_t* t_id=(pthread_t*)calloc(cfg->dn,sizeof(pthread_t));  //unix thread id type
		#endif // __unix__

		Tmst** threads=(Tmst**)calloc(cfg->dn,sizeof(Tmst*)); //data passed into threads
		for(unsigned int i=0;i<cfg->dn;i++){
			//get data for threads
			threads[i] = new_Threadedmsort_argv_t( lineno_,highest, filename, tmp[i],i,cfg->cid[i]);
			//create threads
			#ifdef __WIN32
			t_id_handle[i]=CreateThread(NULL,262144,&msort,threads[i],0,t_id[i]); //262144 bytes for stack alloc ~> 1/4MB
			#endif // __WIN32
			#ifdef __unix__
			pthread_create(&t_id[i],NULL ,msort, threads[i]);
			#endif // __unix__
		}

		#ifdef __WIN32
		WaitForMultipleObjects(cfg->dn,t_id_handle,TRUE,INFINITE);
		#endif // __WIN32
    
		//join threads
		for(int i=0;i<cfg->dn;i++){
        #ifdef __WIN32
            CloseHandle(t_id_handle[i]);
        #endif // __WIN32
        #ifdef __unix__
            pthread_join(t_id[i], NULL);
        #endif // __unix__
        //both (free thread_data)
		del_Threadedmsort_argv_t(threads[i]);// (TODO?)
	}
		free(threads);
		free(t_id);
	}
	FILE* tmp0=NULL;
	//cp first line
	{
		FILE* fp=fopen(filename,"r");
		char* line=(char*)calloc(sizeof(char),(lineno_[0]+2));
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
	}
//cp each file into tmp0.csv
	for(int i=0;i<cfg->dn;i++){
		//maybe use a global cid[i]
		freopen("tmp0.csv","a",tmp0); //maybe this can be deleted
		fprintf(tmp0 ,"====ID:<%s>====\n",cfg->cid[i]);
		appendFILE(tmp0,tmp[i]);
		fclose(tmp[i]);
	}
	//merge end
	fclose(tmp0);
	free(tmp);
	return 0;
}

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
	fseek(th->fp,th->lineno_[0]+1,SEEK_SET);
	do {
		ch=fgets(th->line, th->lineno_[lineno]+2, th->fp);
		//TODO refactor to fputs()
		if(strstr(th->line,th->id)) fprintf(th->tmp,"%s",th->line);
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
	char* string=(char*)calloc(sizeof(char),src_size+1);
	fread(string, src_size, 1, src);
	fprintf(dest,"%s",string);	
	//memory
	fflush(dest);
	free(string);
	return dest;
}

