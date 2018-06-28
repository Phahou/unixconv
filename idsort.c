/* idsort.c: Sorting files bc BUS Systems aren't perfect */

void msort(int id_nums,unsigned int* lineno_,char* line,ln* ln_0,FILE **tmp,fpos_t firstline);

int idsort(const char* filename,int id_nums,int opt, unsigned int* lineno_, unsigned int* highest){ //id_nums: number of IDs
  ec* ec_0=new_ec(CID0); //changeable
  ln* ln_0=new_ln(ec_0);
  ln_0->fp=fopen(filename,"r");

  FILE **tmp	 = (FILE**)malloc(sizeof(FILE*)*id_nums);
  char** del_list= (char**)calloc(sizeof(char*),id_nums);
  char* fl_reset = (char*)malloc(sizeof(char)*strlen(filename));
  char* fl		 = (char*)malloc(sizeof(char)*strlen(filename));
  strcpy(fl,filename);


  if(opt & 8) printf(BOLD WHT "Generating tmp files...\n" RESET);

  //initializing tmp files
  for(int i=0;i<id_nums;i++){
    strcpy(fl_reset,fl);

    char* i_=(char*)calloc(sizeof(char),7);
    sprintf(i_,"_%d.tmp",i);
    tmp[i]=fopen(strcat(fl,i_),"w+");

    del_list[i]=(char*)calloc(sizeof(char),strlen(i_));
    strcpy(del_list[i],fl);

    free(i_);
    strcpy(fl,fl_reset);
  }
  free(fl);
  fpos_t firstline;
  ln_0->skipln(ln_0); //skip 1 lines
  fgetpos(ln_0->fp,&firstline);

  char* line=(char*)calloc(*highest,sizeof(char));
  //sort ids in specific files


  if(opt & 8) printf("...... Sorting <%s>\r",filename);
  msort(id_nums,lineno_,line,ln_0,tmp,firstline);


  if(opt & 8){
    printf( BOLD WHT "[" GRN "done" WHT "]" RESET "\n");
    printf("...... Merging tmp files\r");
  }
//merging ID files
  //cp first line
  line=(char*)realloc(line,sizeof(char)*(lineno_[0]+2));
  fgets(line,lineno_[0]+2,ln_0->fp);

  //closing filename:
  fclose(ln_0->fp);
  FILE* tmp0=fopen("tmp0.csv","w");
  if(tmp0==NULL) perror("Error tmp0.csv: ");
  fprintf(tmp0,"%s",line);
  free(line);

  for(int i=0;i<id_nums;i++){
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
    fflush(tmp[i]);
    free(string);
  }
  free(ln_0);
  free(ec_0);
  if(opt & 8){
    printf( BOLD WHT "[" GRN "done" WHT "]" RESET "\n");
    printf("...... Removing tmp files\r");
  }
  //Removing files
  for(int i=0;i<id_nums;i++){
    remove(del_list[i]);
    free(del_list[i]);
  }
  if(opt & 8) printf( BOLD WHT "[" GRN "done" WHT "]" RESET "\n");
  return 0;
}

void msort(int id_nums,unsigned int* lineno_,char* line,ln* ln_0,FILE **tmp,fpos_t firstline){
  char* ch="b";
  for(int i=0;i<id_nums;i++){
	unsigned int lineno=1;
	ch="b";
	fsetpos(ln_0->fp,&firstline);
	switch(i){
	  case 0: strcpy(ln_0->ecp->id,ID0);
			  break;
	  case 1: strcpy(ln_0->ecp->id,ID1);
			  break;
	  case 2: strcpy(ln_0->ecp->id,ID2);
			  break;
	  case 3: strcpy(ln_0->ecp->id,ID3);
			  break;
	  case 4: strcpy(ln_0->ecp->id,ID4);
			  break;
	  case 5: strcpy(ln_0->ecp->id,ID5);
			  break;
	  case 6: strcpy(ln_0->ecp->id,ID6);
			  break;
	  case 7: strcpy(ln_0->ecp->id,ID7);
			  break;
	  default: fprintf(stderr,"\nError switch(i) line 62 in idsort.c\n"); //never happens
			   exit(-45);
	}
//filtering IDs in each tmp file
	while (ch!=NULL){
	  ch=fgets(line,lineno_[lineno]+2,ln_0->fp);
	  if(strstr(line,ln_0->ecp->id)!=NULL) fprintf(tmp[i],"%s",line);
	  fflush(tmp[i]);
	  lineno++;
	}
  }
}
