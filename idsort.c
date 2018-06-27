/* idsort.c: Sorting files bc BUS Systems aren't perfect */

FILE* skiplines(FILE* fp,int times){ //skip lines x times
  for(int i=0;i<times;i++){
    while(fgetc(fp)!='\n') ;
  }
  return fp;
}

int idsort(const char* filename,int id_nums,int opt){ //id_nums: number of IDs
  ec* ec_0=new_ec(CID0); //changeable
  ln* ln_0=new_ln(ec_0);
  ln_0->fp=fopen(filename,"r");
  char* fl_reset=(char*)malloc(sizeof(char)*strlen(filename));
  FILE **tmp=(FILE**)malloc(sizeof(FILE*)*id_nums);
  char* fl=(char*)malloc(sizeof(char)*strlen(filename));
  strcpy(fl,filename);
  //allocation of strings for file removal at the end of the function
  char** File_deletion_list=(char**)calloc(sizeof(char*),id_nums);
  if(opt & 8) printf(BOLD WHT "Generating tmp files...\n" RESET);
  //initializing tmp files
  for(int i=0;i<id_nums;i++){
    strcpy(fl_reset,fl);
    char* _i=(char*)calloc(sizeof(char),7);
    sprintf(_i,"_%d.tmp",i);
    tmp[i]=fopen(strcat(fl,_i),"w+");
    File_deletion_list[i]=(char*)calloc(sizeof(char),strlen(_i));
    strcpy(File_deletion_list[i],fl);
    free(_i);
    strcpy(fl,fl_reset);
  }
  free(fl);
  fpos_t firstline;
  int line_length=0;
  fseek(ln_0->fp,0,SEEK_SET);
  ln_0->skipln(ln_0); //skip 1 lines
  fgetpos(ln_0->fp,&firstline);

  line_length=ln_0->getlnlen(ln_0); //get length of a line
  char* line=(char*)malloc(sizeof(char)*line_length);
  //sort ids in specific files
  if(opt & 8) printf("...... Sorting <%s>\r",filename);

  //char ID[21];
  for(int i=0;i<id_nums;i++){
    char ch=fgetc(ln_0->fp);
    fsetpos(ln_0->fp,&firstline);
    switch(i){
      case 0: strcpy(ec_0->id,ID0);
              break;
      case 1: strcpy(ec_0->id,ID1);
              break;
      case 2: strcpy(ec_0->id,ID2);
              break;
      case 3: strcpy(ec_0->id,ID3);
              break;
      case 4: strcpy(ec_0->id,ID4);
              break;
      case 5: strcpy(ec_0->id,ID5);
              break;
      case 6: strcpy(ec_0->id,ID6);
              break;
      case 7: strcpy(ec_0->id,ID7);
              break;
      default: fprintf(stderr,"\nError switch(i) line 62 in idsort.c\n"); //never happens
               exit(-45);
    }
    //filtering IDs in each tmp file
    while (ch!=EOF){
      fread(line, line_length+1, 1, ln_0->fp);
      if(strstr(line,ec_0->id)!=NULL) fprintf(tmp[i],"%s",line);
      ch=fgetc(ln_0->fp);
      fseek(ln_0->fp, -1,SEEK_CUR);
    }
  }

  if(opt & 8){
    printf( BOLD WHT "[" GRN "done" WHT "]" RESET "\n");
    printf("...... Merging tmp files\r");
  }
  //merging ID files
  //cp first line

  char first_line[95];
  fseek(ln_0->fp,0,SEEK_SET);
  fread(first_line,95,1,ln_0->fp);

  //closing filename:
  fclose(ln_0->fp);

  FILE* tmp0=fopen("tmp0.csv","w");
  if(tmp0==NULL) perror("Error tmp0.csv: ");
  fprintf(tmp0,"%s",first_line);
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
    }//cp each file into tmp0.csv
    freopen("tmp0.csv","a",tmp0);
    fprintf(tmp0,"====ID:<%s>====\n",ec_0->id);
    fseek(tmp[i], 0, SEEK_END);
    long unsigned tmpsize = ftell(tmp[i]);
    fseek(tmp[i], 0, SEEK_SET);
    char *string =(char*) malloc(tmpsize+1);
    fread(string, tmpsize, 1, tmp[i]);
    //string[tmpsize] = 0;
    fprintf(tmp0,"%s",string);
    //printf("%s",string);
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
    remove(File_deletion_list[i]);
    free(File_deletion_list[i]);
  }
  if(opt & 8) printf( BOLD WHT "[" GRN "done" WHT "]" RESET "\n");
  return 0;
}
