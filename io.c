/* io.c for I/O Operations. Is at the same time the "main" compiling file */

/*TODO:
 * change the reading to read one line and use it in the whole loop instead of moving the pointer always forward
 * remove magic numbers
 */

typedef unsigned short int USI;

#ifdef __WIN32
#include<windows.h>
#include<shlwapi.h>

#define  rename_file    \
    remove(filename);   \
    MoveFileA("file.csv", filename);

#endif // __WIN32

#ifdef __unix__
#include<sys/stat.h>/* needed for recognizing if input is a dir */

#define rename_file \
    rename("file.csv",filename);

#endif // __unix__

/* 1. Includes */
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<stdbool.h>

#include"options.c" // Setting eg -c for added semicolons
#include"idsort.c"  // Sorting IDs in the right order
#include"windows.c" // Converting windows CRLF into unix LF
#include"dirs.c"    // Checking folders recursively

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

    struct cfg_t* cfg=loadcfg();    //Read the config file

    list** files=p_argv(argc,argv,opt);

    USI MaxCharsLine=0, inputed_files=0;
    USI* lineno_=(USI*)calloc(cfg->dn*cfg->ml,sizeof(USI));//= { 0 };

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
                    if ( /*(reducedata((char*)curr_pos->str, opt, cfg)==-1)&&(!triedagain)*/ false ){
                        i--; //try again (error while opening stuff)
                        triedagain=true;
                        //break out of the loop and try again (same head in list )
                        goto TRYAGAIN;
                    }
                    // break bc if it sucessfully reducesdata it will go on without the goto
                    break;
                case 2:
                    fprintf(stderr,
						BOLD WHT "(Skip)"
						RESET "Something seems wrong with this file: %s\n",
						(char*)curr_pos->str);
                    break;
            }

            curr_pos=curr_pos->next;
        }
        TRYAGAIN: //skip curr_pos=curr_pos->next;
        if(!triedagain) del_complete_list(files[i]);
    }
    free(lineno_);
    del_cfg(cfg);

    free(files); //free the array we used in p_argv;
    /* DEBUG COMMENTED
    if (fopen("tmp0.csv","r")) {
        if(opt & 8) printf("Cleaning up tmp0.csv\n");
        remove("tmp0.csv");
    }*/
    return 0;
}

int reducedata(const char* filename, int opt, struct cfg_t* cfg){
    ln* lp =new_ln(new_ec(NULL));   //line-pointer
    lp->fp=fopen("tmp0.csv","r");
    if(!lp->fp){        //Error while Opening stuff
        del_ln(lp);
        return -1;
    }

    if(fileinit(lp,opt)==-1) return -1;

//tmp file is open now
    fseek(lp->fp,0,SEEK_SET);
    printfirstline(lp);
    if(opt & 8) printf("Calcing <%s>\n",filename);

/*
int haltbeisemikolon=1;
loop:
int haltbeisemikolon=1;
  if(ENDE) goto EXIT;
  if(lineskip){
    print "skipline";
    goto loop;
  }
  string time=gettime();
  semikolons++;
  string values=getvalues();
  int diff=calc_diff(values1,values2);
  print "time;values;diff"
goto loop;
EXIT:
*/

//Overview:
//-> TIME;"ID";"VALUE";R4ND0M_$H!T'\n'
//data conversion -> get value & print value
        //just use a strchr lol

//calc_diff
{
	int calc_status=lp->calc_diff(lp->ecp->value, lp);
	switch(calc_status){
		case -10:/* fall through */
		case -1: reachedEOF(lp, calc_status, opt);
		default: break;
	}

    fprintf(lp->ecp->tmp,"%lu",lp->diff);

	switch(lp->diff){
		case 0: /* fall through */
		case 1: break;
		default:fprintf(lp->ecp->tmp,";%lu",lp->diff);
	}
	fprintf(lp->ecp->tmp,"\n");

//save line
    fflush(lp->ecp->tmp);
	fflush(lp->fp);
}

    fclose(lp->ecp->tmp);
    fclose(lp->fp);
    del_ln(lp);

    rename_file; //rename "file.csv" to filename
    remove("tmp0.csv");

    printf(BOLD WHT "[" GRN "done" WHT "]" RESET " %s\n",filename);
    return 0;
}

/* ----------------------------4. Helper functions ---------------------------*/

int idhasntchanged(char* id){
    return strncmp(id,"====",4);
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
    fprintf(lpr->ecp->tmp,
"\"Epochzeit\";\"Zaehler-IDs\";\"Zaehlerwert\";""\"Normale Zeit\";"
"\"Stromdifferenz zwischen den Messungen\";\"Stromdifferenz ohne 0 & 1\"\n");
    fflush(lpr->ecp->tmp);
}

list** p_argv(int argc, char** argv,int opt){
    unsigned short int files_entered=0;
    for (int i=1;i<argc;i++){ //check if input isn't an option
        if (argv[i][0]!='-') files_entered++;
    }
    if(files_entered==0){
        fprintf(stderr,"No files selected!\n");
        exit(1);
    }
    //check the arguments
    //printf("valid: %d",files_entered);
    list** argdir=(list**)calloc(sizeof(list*),files_entered);     //argv with dirs
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
