/* io.c for I/O Operations. Is at the same time the "main" compiling file */

/*TODO:
 * change the reading to read one line and use it in the whole loop instead of moving the pointer always forward
 * remove magic numbers
 */

#ifdef __WIN32
#include<windows.h>
#include<shlwapi.h>
#endif // __WIN32

#ifdef __unix__
#include<sys/stat.h>/* needed for recognizing if input is a dir */
#endif // __unix__

/* 1. Includes */
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<stdbool.h>

#include"options.c" 	// Setting eg -c for added semicolons
#include"idsort.c"	// Sorting IDs in the right order
#include"windows.c"	// Converting windows CRLF into unix LF
#include"dirs.c"	// Checking folders recursively

/* 2. function definitions */
int idhasntchanged(char* id);
int reducedata(const char* filename, int opt, struct cfg_t* cfg);
int isequalcheck(ln* lpr, fpos_t *pos);
int fileinit(ln* lpr, int opt);
void reachedEOF(ln* lpr, int status, int opt);
void printfirstline(ln* lpr);
bool printhelp(int opt, int argc);
list** p_argv(int argc,char** argv,int opt);
int unixconv_main(int argc,char** argv, int _opt, bool isqt);

/* 3. main function */
int main(int argc,char** argv){
	int _opt=0;
	bool isqt=false;
	int status=unixconv_main(argc,argv,_opt,isqt);
	return status;
}

int unixconv_main(int argc,char** argv, int _opt, bool isqt){
    int opt=0;
    if (isqt) opt=_opt;     //if executed in a gui isqt is true
    else    opt=p_options(argc,argv);

    struct cfg_t* cfg=readcfg();    //Read the config file
    
    list** files=p_argv(argc,argv,opt);
    unsigned short int MaxCharsLine=0, inputed_files=0;
    //TODO: make this a buffer or dynamic but then it would decrease performance :P
    unsigned short int lineno_[cfg->dn*cfg->ml];//= { 0 };

    for (int i=1;i<argc;i++) if (argv[i][0]!='-') inputed_files++;

    for(int i=0;i<inputed_files;i++) {
		//check if it is already converted 1 = no 0 = yes
        bool triedagain=false;
        list* curr_pos=files[i];
        while(curr_pos){
            FILE* fp=fopen((char*)curr_pos->str,"r");
            int not_converted=alreadyconverted((char*)curr_pos->str, fp, opt);
            fclose(fp);
            switch (not_converted) {
                case 0:
                    //do nothing because it is already converted
                    break;
                case 1:
                    //opt & 2 has to be true bc if not lineno is 0
                    if(opt & 2) rmwinCRLF((char*)curr_pos->str, lineno_, &MaxCharsLine); //converting windows CRLF into unix LF
                    idsort((char*)curr_pos->str, opt, lineno_, &MaxCharsLine,cfg); //sort IDs in the right order
                    if ( (reducedata((char*)curr_pos->str, opt, cfg)==-1)&&(!triedagain) ){
                        i--; //try again (error while opening stuff)
                        triedagain=true;
                        //break out of the loop and try again (same head in list )
                        goto TRYAGAIN;
                    }
                    // break bc if it sucessfully reducesdata it will go on without the goto
                    break;
                case 2:
                    fprintf(stderr, BOLD WHT "(Skip)" RESET "Something seems wrong with this file: %s\n",(char*)curr_pos->str);
                    break;
            }
            curr_pos=curr_pos->next;
        }
        TRYAGAIN: ;
        if(!triedagain) del_complete_list(files[i]);
    }
    free(files); //free the array we used in p_argv;
    if (fopen("tmp0.csv","r")) {
        if(opt & 8) printf("Cleaning up tmp0.csv\n");
        remove("tmp0.csv");
    }
    return 0;
}


int idhasntchanged(char* id){
	return strncmp(id,"====",4);
}

int reducedata(const char* filename, int opt, struct cfg_t* cfg){
	ln* lp =new_ln(new_ec(NULL));	//line-pointer
	lp->fp=fopen("tmp0.csv","r");
	if(!lp->fp){		//Error while Opening stuff
		del_ln(lp);
		return -1;
	}

	//okay the file isnt reduced yet...
	if(fileinit(lp,opt)==-1) return -1;
	//tmp file is open now
	fpos_t pos;
	fseek(lp->fp,0,SEEK_SET);
	printfirstline(lp);

	lp->diff=0;
	int status;
	if(opt & 8) printf("Reducing <%s>\n",filename);
	for(int done_ids=0;done_ids<=cfg->dn;done_ids++){
		while(1){
			status=isequalcheck(lp, &pos);
			if(status==-1) break;
			else if (status==-2) lp->skipln(lp);

//get data:
			fgets(lp->ecp->time,11,lp->fp);
			fprintf(lp->ecp->tmp,"%s",lp->ecp->time);
			fprintf(lp->ecp->tmp,"%c",fgetc(lp->fp) );

//Rewriting IDs
			fgets(lp->ecp->id,21,lp->fp);	//id

//compare IDs and print them in tmp file
			if(opt & 8) lp->ecp->setID(lp->ecp,cfg->cid[done_ids-1]);
                        lp->ecp->printID(lp->ecp, opt & 1, done_ids-1);
			fprintf(lp->ecp->tmp,"%c",fgetc(lp->fp));	//Copy Values

			fgetc(lp->fp);	//Without ""
			fgetpos(lp->fp,&pos);
			int i=0;
			char ch='0';
			for(i=0; ch!='"' && ch!=EOF;i++) ch=fgetc(lp->fp);
			fsetpos(lp->fp,&pos);

//data conversion -> get value & print value
			char* values=(char*)calloc(sizeof(char),i);
			for(int j=0;j<i;j++) values[j]=fgetc(lp->fp);
			lp->ecp->value=strtoul(values,NULL,10);
			free(values);
			fprintf(lp->ecp->tmp,"%lu;",lp->ecp->value);

//check if fp is on the right pos
			fgetc(lp->fp); //skip "
			fgetc(lp->fp); //skip ;

//time
			lp->ecp->convertedTime(lp->ecp);
			fprintf(lp->ecp->tmp,"\"%s\";",lp->ecp->time_readable); //Print time

//calc_diff
			int calc_status=lp->calc_diff(lp->ecp->value, lp);

			if(calc_status==-10){
				reachedEOF(lp, calc_status, opt);
				break;
			}
			if(calc_status==-1){
				reachedEOF(lp, calc_status, opt);
				done_ids--;
				break;
			}

			fprintf(lp->ecp->tmp,"%lu",lp->diff);

			if( (lp->diff != 0) &&
				(lp->diff != 1) ) fprintf(lp->ecp->tmp,";%lu",lp->diff);

			fprintf(lp->ecp->tmp,"\n");
//save line
			fflush(lp->ecp->tmp);
			fflush(lp->fp);
		}
	}
	fclose(lp->ecp->tmp);
	fclose(lp->fp);
	del_ln(lp);
	#ifdef __unix__
	rename("file.csv",filename);
	#endif // __unix__
	#ifdef __WIN32
	remove(filename);
        MoveFileA("file.csv", filename);
	#endif // __WIN32
	remove("tmp0.csv");
	printf(BOLD WHT "[" GRN "done" WHT "]" RESET " %s\n",filename);
	return 0;
}

/* 4. Helper functions */
int isequalcheck(ln* lpr, fpos_t *pos){
	int i=0;
	lpr->skipln(lpr);

	fgetpos(lpr->fp,pos);
	char isequal=fgetc(lpr->fp);
	if(isequal=='='){	//print rest of line e.g. ===ID: A===
		fsetpos(lpr->fp,pos);
		i=lpr->getlnlen(lpr);
		fsetpos(lpr->fp,pos);
		char* idline=(char*)malloc(sizeof(char)*i);
		fgets(idline,i,lpr->fp);
		fprintf(lpr->ecp->tmp,"%s\n",idline);
		free(idline);
		return -1;
	} else if (isequal=='\n') return -2;
	fsetpos(lpr->fp,pos);
	return 0;
}

int fileinit(ln* lpr, int opt){
	if(opt & 8) printf("...... Opening files for conversion\r");
	lpr->ecp->tmp=fopen("file.csv","w+");
	if(!lpr->ecp->tmp){
		perror("\nError file.csv:");
		return -1;
	}
	if(opt & 8) printf(BOLD WHT "[" GRN "done" WHT "]" RESET "\n");
	return 0;
}

void reachedEOF(ln* lpr, int status, int opt){
	if(opt & 8){
		printf("Reached End of Input for <%s>\n",lpr->ecp->cid);
	}
	//printing a 0 for diff bc we have only 1 value
	if(status==-1) fprintf(lpr->ecp->tmp,"0\n");
	fflush(lpr->ecp->tmp);
	fflush(lpr->fp);
}

void printfirstline(ln* lpr){
	fprintf(lpr->ecp->tmp,"\"Epochzeit\";\"Zaehler-IDs\";\"Zaehlerwert\";");
	fprintf(lpr->ecp->tmp,"\"Normale Zeit\";\"Stromdifferenz zwischen den ");
	fprintf(lpr->ecp->tmp,"Messungen\";\"Stromdifferenz ohne 0 & 1\"\n");
	fflush(lpr->ecp->tmp);
}

list** p_argv(int argc, char** argv,int opt){
    unsigned short int valid_arguments=0;
    for (int i=1;i<argc;i++){ //check if input isn't an option
        if (argv[i][0]!='-') valid_arguments++;
    }
    if(valid_arguments==0){
        fprintf(stderr,"No files selected!\n");
        exit(1);
    }
    //check the arguments
    printf("valid: %d",valid_arguments);
    list** argdir=(list**)calloc(sizeof(list*),valid_arguments);     //argv with dirs
    //aka: list* argdir[valid_arguments]
    int k=0;
    for (int i=1;i<argc;i++){ //go through argv
        if (argv[i][0]=='-') continue;
        else if(isdir(argv[i])) {
            if(opt & 16) {
                argdir[k++]=dirs(argv[i],true,ENDPATTERN);
                if(!argdir[k-1]) k--; // -> Ensures that NULL is the end of the list
            } else {
                fprintf(stderr,"'%s' is a directory.\n", argv[i]);
            }
        } else {
            argdir[k++]=addlast(new_list(argv[i],true),NULL);
        }
    }
    return argdir;
}
