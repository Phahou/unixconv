/* io.c for I/O Operations. Is at the same time the "main" compiling file */

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

struct device_t {
    //line based
    //only holds pointers!
    char** time;
    char** value;
    char** diff;
};

struct device_t* new_device(size_t sum_of_newlines){
    struct device_t* obj=(struct device_t*)calloc(1,sizeof(struct device_t));
    obj->time=(char**) calloc(sum_of_newlines,sizeof(char*));
    obj->value=(char**)calloc(sum_of_newlines,sizeof(char*));
    obj->diff=(char**) calloc(sum_of_newlines,sizeof(char*));
    return obj;
}

void del_device(struct device_t* device, size_t sum_of_newlines){
    if(device){
        if(device->time)    free(device->time);
        if(device->value)   free(device->value);
        if(device->diff){
            for(size_t i=0;i<sum_of_newlines;i++){ 
                switch(device->diff[i][0]){           
                    case '1': /* fall through */ // ignore "1" & "0" strings
                    case '0':                    // bc they are already alloc'd
                        if(device->diff[i][1]=='\0') break;
                        else /* fall through */
                    default: free(device->diff[i]);
                }
            }
            free(device->diff);
        }
        free(device);
    }
}

/* 2. function definitions */
int reducedata(const char* filename, int opt, struct cfg_t* cfg, size_t* offsetcid);
int unixconv_main(int argc,char** argv, int _opt, bool isqt);

/* Helpers */
list** p_argv(int argc,char** argv,int opt);


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

    struct cfg_t* cfg=loadcfg();    //Load the config file

    list** files=p_argv(argc,argv,opt);

    size_t highest=0; //Highest number of chars of all lines (used for allocing)
    size_t* offsetcid=(size_t*)calloc( cfg->dn , sizeof(size_t) );
    USI inputed_files=0;
    for (int i=1;i<argc;i++) if (argv[i][0]!='-') inputed_files++;

    for(int i=0;i<inputed_files;i++) {
        //check if it is already converted 1 = no 0 = yes
        bool triedagain=false;
        list* curr_pos=files[i];
        while(curr_pos){
            FILE* fp=fopen((char*)curr_pos->str,"r");
            int not_converted;
            if (fp) {
                not_converted=alreadyconverted((char*)curr_pos->str, fp, opt, cfg);
                fclose(fp);
            }

            switch (not_converted) {
                case 0:
                    //do nothing because it is already converted
                    break;
                case 1:
                    rmwinCRLF_and_fill((char*)curr_pos->str, &highest); //converting windows CRLF into unix LF
                    idsort((char*)curr_pos->str, opt, &highest, cfg, offsetcid); //sort IDs in the right order
                    if ( (reducedata((char*)curr_pos->str, opt, cfg, offsetcid)==-1)&&(!triedagain) ){
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
{ //clean up
    del_cfg(cfg);
    free(offsetcid);
    free(files); //free the array we used in p_argv;
}
    /* DEBUG COMMENTED
    if (fopen("tmp0.csv","r")) {
        if(opt & 8) printf("Cleaning up tmp0.csv\n");
        remove("tmp0.csv");
    }*/
    return 0;
}


int reducedata(const char* filename, int opt,
struct cfg_t* cfg, size_t* offsetcid){ //offsetcid: offset of the devices
    ln* lp;
//TODO: rename lp->fp to lp->tmp or better lp->fp_in lp->fp_out
{ //boilerplate variable inits
  { // initialize lp (set filepointer) do error handling
      lp=new_ln(new_ec());   //line-pointer
      lp->fp=fopen("tmp0.csv","r");
      if(!lp->fp){        //Error while Opening stuff
          del_ln(lp);
          return -1;
      }
  }

  {// initialize output file.
      if(opt & 8) printf("...... Opening files for conversion\r");
      lp->ecp->tmp=fopen("file.csv","w+");
      if(!lp->ecp->tmp){
          perror("\nError file.csv:");
          return -1;
      }
      if(opt & 8) printf(BOLD WHT "[" GRN "done" WHT "]" RESET "\n");

  //print first line
      fprintf(lp->ecp->tmp,"\"Epochzeit\"");
      for(unsigned short int i=0;i<cfg->dn;i++){
          fprintf(lp->ecp->tmp,";%s;->DIFF",cfg->cid[i]);
      }
  }
}
    char* str=NULL; // will be freed later
{ // read file into str pretty much like a function call
    FILE* src=lp->fp;
    fseek(src, 0, SEEK_END);
    size_t src_size = ftell(src);
    fseek(src, 0, SEEK_SET);
    char* string=(char*)calloc(src_size+1,sizeof(char));
    fread(string, src_size, 1, src);
    str=string; //"returns" here
    src=NULL;   //set src to NULL for safety
}
    char** device=NULL;
{ // init device start pointers idk
    device=(char**)calloc(cfg->dn,sizeof(char*));
    for(size_t i=0;i<cfg->dn;i++) {
        device[i]=&str[ offsetcid[i] ];
        if((i+1)<cfg->dn) str[ offsetcid[i+1]-1 ]='\0'; //cut device strings
    }
}
    bool allok=true;
    size_t* newlines=NULL;
{ // check if both have the same number of rows
//get row numbers of each device
    newlines=(size_t*)calloc(cfg->dn,sizeof(size_t));
    for(size_t i=0;i<cfg->dn;i++) {
        char* tmp_ch=NULL;
        char* ch=device[i];
        while( (tmp_ch=strchr(ch,'\n')) ){
            if(tmp_ch[1]) {
                ch=&tmp_ch[1];
                newlines[i]=newlines[i]+1;
            } else break;
        }
    }
//all the same?
    for(size_t i=1;i<cfg->dn;i++) {
        if(newlines[i]!=newlines[0]) allok=false;
        //printf("newlines[%ld]=%ld\n",i,newlines[i]);
    }

}

    struct device_t** devs=(struct device_t**)calloc(cfg->dn,sizeof(struct device_t*));

    char* nullvalue=(char*)calloc(2,sizeof(char)); // for later use with devs
    char* onevalue=(char*)calloc(2,sizeof(char));
    nullvalue[0]='0';
    onevalue[0]='1';

{ //calc differences and combine devices into one line
    if(!allok) {
        fprintf(stderr,"Exception is not yet implemented sorry!\n");
        exit(-1);
    }
    if(opt & 8) printf("Calcing differences for <%s>\n",filename);

    for(size_t i=0;i<cfg->dn;i++) {     //gen a type of hashmap for the values &
        devs[i]=new_device(newlines[i]);//stuff it's easier this way (I think)
    }

    { //get diff and fill devs[i] structure
    unsigned int val1, val2, diff;

    for(size_t i=0;i<cfg->dn;i++) {
        char* oldrow=strchr(device[i],'\n');
        char* val, *cut;
        if(oldrow && oldrow[1]) oldrow=&oldrow[1]; //skip this ==ID:<>== stuff
        { //get val0 first iteration by hand
        devs[i]->time[0]=oldrow;
        cut=strchr(oldrow,';'); //cut time
        cut[0]='\0';
        val=&cut[1];        devs[i]->value[0]=val;
        cut=strchr(val,'\n'); //cut val
        cut[0]='\0';
        val1=atoi(val);
        devs[i]->diff[0]=nullvalue; //as there is nothing before it in this file

        if(cut[1]) oldrow=&cut[1];
        else break;
        }
        for(size_t l=1;l<newlines[i];l++){ // get other vals

            devs[i]->time[l]=oldrow;
            cut=strchr(oldrow,';'); //cut time
            cut[0]='\0';

            val=&cut[1];
            cut=strchr(val,'\n'); //cut val
            if(cut==0) { //reached the End
                devs[i]->value[l]=val;
                devs[i]->diff[l]=nullvalue;
                break;
            } cut[0]='\0';
            val2=atoi(val);         devs[i]->value[l]=val;

            diff=val2-val1;//calc diff: val2 is always bigger than val1
            { //set diff pointer
            if(diff==0) devs[i]->diff[l]=nullvalue;
            else if(diff==1) devs[i]->diff[l]=onevalue;
            else { //alloc space
                unsigned int v1=atoi(devs[i]->value[l-1]);
                unsigned int v2=atoi(devs[i]->value[l]);
                unsigned int max=(v1>v2)? v1: v2;
                unsigned int size=(v1==max)?
                    strlen(devs[i]->value[l-1]): //v1
                    strlen(devs[i]->value[l]); //v2
                devs[i]->diff[l]=(char*)calloc(size+1,sizeof(char));
                sprintf(devs[i]->diff[l],"%u",diff);
            }
            }
            if(cut[1]) oldrow=&cut[1]; //advance row
            else { //Reached end normally not needed bc l<newlines[i]
                diff=0; //nothing here so nothing is the diff
                devs[i]->diff[l++]=nullvalue;
                break;
            }
            val1=val2; //reset val1 for next iteration
        }
    }
    } //end get diff and fill devs[i] structure
}
{ //print into output file
    for(size_t l=0;l<newlines[0];l++){
        fprintf(lp->ecp->tmp,"\n%s",devs[0]->time[l]);
        char* log=devs[0]->time[l]; //readablity
        for(size_t i=0;i<cfg->dn;i++) {
            char* logged=devs[i]->time[l]; // readablity
            if ( (atoi(log)-cfg->tol)<=(atoi(logged)) && //x<=y<=z
                (atoi(logged)<=(atoi(log)+cfg->tol)) ){
                fprintf(lp->ecp->tmp,";%s;%s",devs[i]->value[l], devs[i]->diff[l]);
            }
        }
    }
}
{//clean up
    del_ln(lp);
    for(size_t i=0;i<cfg->dn;i++) {
        del_device(devs[i],newlines[i]);
    }
    free(newlines);
    free(devs);
    free(nullvalue);
    free(onevalue);

    free(str);
    free(device);
//    rename_file; //rename "file.csv" to filename
//    remove("tmp0.csv");
}
    printf("\r" BOLD WHT "[" GRN "done" WHT "]" RESET " %s\n",filename);
    return 0;
}

/* ----------------------------4. Helper functions ---------------------------*/

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
