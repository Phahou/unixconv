/* io.c for I/O Operations. Is at the same time the "main" compiling file */

/* ToC
 * 1. Includes
 * -> POSIX libs
 * -> Own Includes (with OO Concepts)
 * 2. function definitions
 * 3. main function
 * 4. Helper functions
 */

/*TODO:
 * change the reading to read one line and use it in the whole loop instead of moving the pointer always forward
 * remove magic numbers
 */

/* 1. Includes */
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<stdbool.h>
#include<sys/stat.h>/* needed for recognizing if input is a dir */
#include<pthread.h> /* for multithreading in idsort()			*/

/* Own Includes: */
#include"line.c"	/* data-structures holding important infos	*/
#include"options.c" /* for setting eg -r for only reducing		*/
#include"config.c"	/* Custom Config for defining ID Names etc. */
#include"idsort.c"	/* for sorting IDs in the right order		*/
					/* ->after ID1 is ID2 in the next line		*/
					/*	 and so on								*/
#include"windows.c"	/* converts windows CRLF into unix LF		*/
#include"dirs.c"	/* for checking folders recursively			*/

/* 2. function definitions */
int idhasntchanged(char* id);
int reducedata(const char* filename, int opt);
int isequalcheck(ln* lpr, fpos_t *pos);
int fileinit(ln* lpr, int opt);
void reachedEOF(ln* lpr, int status, int opt);
void printfirstline(ln* lpr);
bool printhelp(int opt, int argc);
list** p_argv(int argc,char** argv,int opt);

/* 3. main function */
int main(int argc,char** argv){
    int opt=p_options(argc,argv);
    list** files=p_argv(argc,argv,opt);
    unsigned short int MaxCharsLine=0, inputed_files=0;
    unsigned short int lineno_[MAX_LINE_NUMBERS_PER_DEVICE*INSTALLED_IDS]= { 0 };

    for (int i=1;i<argc;i++) if (argv[i][0]!='-') inputed_files++;

    for(int i=0;i<inputed_files;i++) {
		//check if it is already converted 1 = no 0 = yes
        bool triedagain=false;
        list* curr_pos=files[i];
        while(curr_pos){
            FILE* fp=fopen(curr_pos->str,"r");
            int not_converted=alreadyconverted(curr_pos->str, fp, opt);
            fclose(fp);
            switch (not_converted) {
                case 0:
                    /*do nothing because it is already converted*/
                    break;
                case 1:
                    if(opt & 2) rmwinCRLF(curr_pos->str, lineno_, &MaxCharsLine); //converting windows CRLF into unix LF
                    idsort(curr_pos->str, opt, lineno_, &MaxCharsLine); //sort IDs in the right order
                    if ( (reducedata(curr_pos->str, opt)==-1)&&(!triedagain) ){
                        i--; //try again (error while opening stuff)
                        triedagain=true;
                        /*break out of the loop and try again (same head in list )*/
                        goto TRYAGAIN;
                    }
                    /* break bc if it sucessfully reducesdata it will go on without the goto */
                    break;
                case 2:
                    fprintf(stderr, BOLD WHT "(Skip)" RESET "Something seems wrong with this file: %s\n",(char*)curr_pos->str);
                    break;
            }
            curr_pos=curr_pos->next;
        }
        TRYAGAIN: ;

    }
    if (fopen("tmp0.csv","r")) {
        if(opt & 8) printf("Cleaning up tmp0.csv\n");
        remove("tmp0.csv");
    }
	return 0;
}


int idhasntchanged(char* id){
	return strncmp(id,"====",4);
}

int reducedata(const char* filename, int opt){
	ln* lp =new_ln(new_ec(CID0));	//line-pointer
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
	for(int done_ids=0;done_ids<=INSTALLED_IDS;done_ids++){
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
			lp->ecp->printID(lp->ecp, opt & 1, done_ids-1);
			if(opt & 8) lp->ecp->setID(lp->ecp,done_ids-1);
			fprintf(lp->ecp->tmp,"%c",fgetc(lp->fp));	//Copy Values

			fgetc(lp->fp);	//Without ""
			fgetpos(lp->fp,&pos);
			int i=0;
			char ch='0';
			for(i=0; ch!='"' && ch!=EOF;i++) ch=fgetc(lp->fp);
			fsetpos(lp->fp,&pos);

//data conversion -> get value & print value
			char* values=(char*)calloc(sizeof(char),i);
			for(int j=0;j<i;j++){
				values[j]=fgetc(lp->fp);
			}
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
	rename("file.csv",filename);
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
		printf("Reached End of Input for <%s>\n",lpr->ecp->CustomID);
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
    list** argdir=(list**)calloc(sizeof(list*),valid_arguments);     //argv with dirs
    //aka: list* argdir[valid_arguments]
    int k=0;
    for (int i=1;i<argc;i++){ //go through argv
        if (argv[i][0]=='-') continue;
        else if(isdir(argv[i])) {
            if(opt & 16) {
                argdir[k++]=dirs(argv[i],true,ENDPATTERN);
                if(!argdir[k]) k--; // -> Ensures that NULL is the end of the list
            } else {
                fprintf(stderr,"'%s' is a directory.\n", argv[i]);
            }
        } else {
            argdir[k++]=addlast(new_list(argv[i]),NULL);
        }
    }
    return argdir;
}
