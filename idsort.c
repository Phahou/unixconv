/* idsort.c: Sorting files bc BUS Systems aren't perfect */

#include"Tmsort.c"
void* msort(void* th_);

int idsort(const char* filename,int opt, unsigned int** lineno_, unsigned int* highest){ //id_nums: number of IDs
  static unsigned int k=0;
  ec* ec_0=new_ec(CID0); //changeable
  ln* ln_0=new_ln(ec_0);
  ln_0->fp=fopen(filename,"r");

  //checking if already converted
  int status=alreadyconverted(filename, ln_0->fp, opt);
  if(status == 0) 		return 0;     //skip function nothing to do here
  else if(status == -1) return -1;    //Error while Opening stuff
  //ok not converted already continue function

  FILE **tmp	 = (FILE**)malloc(sizeof(FILE*)*INSTALLED_IDS);
  char** del_list= (char**)calloc(sizeof(char*),INSTALLED_IDS);
  char* fl_reset = (char*)malloc(sizeof(char)*strlen(filename));
  char* fl		 = (char*)malloc(sizeof(char)*strlen(filename));
  strcpy(fl,filename);


  //if(opt & 8) printf(BOLD WHT "Generating tmp files...\n" RESET);

  //initializing tmp files
  for(int i=0;i<INSTALLED_IDS;i++){
    strcpy(fl_reset,fl);

    char* i_=(char*)calloc(7,sizeof(char));
    sprintf(i_,"_%d.tmp",i);
    tmp[i]=fopen(strcat(fl,i_),"w+");
    //strcat(fl,i_);
    del_list[i]=(char*)calloc(sizeof(char),strlen(i_));
    strcpy(del_list[i],fl);

    free(i_);
    strcpy(fl,fl_reset);
  }
  free(fl);
  fpos_t* firstline=(fpos_t*)malloc(sizeof(fpos_t));
  ln_0->skipln(ln_0); //skip 1 lines
  fgetpos(ln_0->fp,firstline);

  char* line=(char*)calloc(*highest,sizeof(char));
  //sort ids in specific files










  if(opt & 8) printf("...... Sorting <%s>\r",filename);
  pthread_t t_id[INSTALLED_IDS/*-1*/];
  Tmst** threads=(Tmst**)malloc(INSTALLED_IDS*sizeof(Tmst*));
  //Tmsort_argv a;
  for(int i=0;i<INSTALLED_IDS;i++){
    //create threads
	threads[i] = new_Threadedmsort_argv_t(lineno_[k],highest,filename,tmp[i],i,firstline);
	//if(i==0) continue;		//skip work for thread 1
	pthread_create(&t_id[i/*-1*/],NULL /*(default)*/,msort, threads[i]);
	//pthread_join(t_id[i], NULL);
	//sort
  }

  //pthread_create(&t_id[0],NULL /*(default)*/,msort, threads[1]);
  //msort(threads[0]);

  //doing sth different to buy the threads more time


  //join threads
  for(int i=0;i<INSTALLED_IDS;i++){
	pthread_join(t_id[i], NULL);
    //del_Threadedmsort_argv_t(threads[i]);
  }
  free(firstline);




  //merging ID files
    //cp first line
  line=(char*)realloc(line,sizeof(char)*(lineno_[k][0]+2));
  fseek(ln_0->fp,0,SEEK_SET);
  fgets(line,lineno_[k][0]+2,ln_0->fp);

  //closing filename:
  fclose(ln_0->fp);

  FILE* tmp0=fopen("tmp0.csv","w");
  if(tmp0==NULL) perror("Error tmp0.csv: ");
  fprintf(tmp0,"%s",line);
  free(line);




  if(opt & 8){
    printf( BOLD WHT "[" GRN "done" WHT "]" RESET "\n");
    printf("...... Merging tmp files\r");
  }

  for(int i=0;i<INSTALLED_IDS;i++){
    switch(i){
      case 0: strcpy(ec_0->id,CID0);
              break;
      case 1: strcpy(ec_0->id,CID1);
              break;
      case 2: strcpy(ec_0->id,CID2);
              break;
      case 3: strcpy(ec_0->id,CID3);
              break;
      case 4: strcpy(ec_0->id,CID4);
              break;
      case 5: strcpy(ec_0->id,CID5);
              break;
      case 6: strcpy(ec_0->id,CID6);
              break;
      case 7: strcpy(ec_0->id,CID7);
              break;
      default: fprintf(stderr,"\nError switch(i) line 89 in idsort.c\n"); //never happens
               exit(-45);
    }
//cp each file into tmp0.csv
    freopen("tmp0.csv","a",tmp0);
    fprintf(tmp0,"====ID:<%s>====\n",ec_0->id);

    fseek(tmp[i], 0, SEEK_END);
    long unsigned tmpsize = ftell(tmp[i]);
    fseek(tmp[i], 0, SEEK_SET);
    char *string =(char*) malloc(tmpsize+1);
    fread(string, tmpsize, 1, tmp[i]);
    fprintf(tmp0,"%s",string);

    fflush(tmp0);
    fclose(tmp[i]);
    free(string);
  }
  free(ln_0);
  free(ec_0);
  if(opt & 8){
    printf( BOLD WHT "[" GRN "done" WHT "]" RESET "\n");
    printf("...... Removing tmp files\r");
  }
  //Removing files
  for(int i=0;i<INSTALLED_IDS;i++){
    remove(del_list[i]);
    free(del_list[i]);
  }
  if(opt & 8) printf( BOLD WHT "[" GRN "done" WHT "]" RESET "\n");
  k++;
  return 0;
}

void* msort(void* th_){
  Tmst* th=(Tmst*)th_;
  char* ch="b";
  unsigned int lineno=1;
  //fsetpos(th->ln_0->fp,th->pos);
  fseek(th->ln_0->fp,th->lineno_[0]+1,SEEK_SET); //skips the first line +2 bc \n and first letter in newline
	switch(th->i){
	  case 0: strcpy(th->ln_0->ecp->id,ID0);
			  break;
	  case 1: strcpy(th->ln_0->ecp->id,ID1);
			  break;
	  case 2: strcpy(th->ln_0->ecp->id,ID2);
			  break;
	  case 3: strcpy(th->ln_0->ecp->id,ID3);
			  break;
	  case 4: strcpy(th->ln_0->ecp->id,ID4);
			  break;
	  case 5: strcpy(th->ln_0->ecp->id,ID5);
			  break;
	  case 6: strcpy(th->ln_0->ecp->id,ID6);
			  break;
	  case 7: strcpy(th->ln_0->ecp->id,ID7);
			  break;
	  default: fprintf(stderr,"\nError switch(i) line 62 in idsort.c\n"); //never happens
			   exit(-45);
	}
//filtering IDs in each tmp file
  unsigned int err_no_ids_found	= 0;
  unsigned int lim				= (INSTALLED_IDS*3)+1;
  bool called=true;

	while ((ch!=NULL)){
	  //warning
	  if((err_no_ids_found>lim)&&(called)){
		  fprintf(stderr,
				  "The string <%s> wasn't found in the file until line(%ud) are you "
				  "sure you entered the correct file or the correct ID in config.c?\n",
				  th->ln_0->ecp->id, (INSTALLED_IDS*3)+2);
		  fprintf(stderr,
				  "The program will continue to work but won't show this error for\n"
				  "this tmpfile again.\n");
		  called=false;
	  }
	  ch=fgets(th->line,th->lineno_[lineno]+2,th->ln_0->fp);

	  if(strstr(th->line,th->ln_0->ecp->id)!=NULL) {
		  fprintf(th->ln_0->ecp->tmp,"%s",th->line);
		  err_no_ids_found=0;
	  } else {
		  err_no_ids_found++;
	  }

	  fflush(th->ln_0->ecp->tmp);
	  lineno++;
	}
	//fclose(th->ln_0->ecp->tmp);
	pthread_exit(0);
}
