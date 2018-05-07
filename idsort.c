//idsort.c for sorting the files bc of the BUS System in MUC_Easy isn't perfect

FILE* skiplines(FILE* fp,int times){ //skip lines x times
  for(int i=0;i<times;i++){
    while(fgetc(fp)!='\n') ;
  }
  return fp;
}

int idsort(const char* filename,int id_num,int opt){ //id_num: number of IDs
  FILE *fp=fopen(filename,"r");
  char* fl_reset=(char*)malloc(sizeof(char)*strlen(filename));
  FILE **tmp=(FILE**)malloc(sizeof(FILE*)*id_num);
  char* fl=(char*)malloc(sizeof(char)*strlen(filename));
  strcpy(fl,filename);
  //allocation of strings for file removal at the end of the function
  char** File_deletion_list=(char**)calloc(sizeof(char*),id_num);
  if(opt & 8) printf(BOLD WHT "Generating tmp files...\n" RESET);
  //initializing tmp files
  for(int i=0;i<id_num;i++){
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
  fseek(fp,0,SEEK_SET);
  fp=skiplines(fp,1); //skip 1 lines
  fgetpos(fp,&firstline);

  while(fgetc(fp)!='\n') line_length++; //get length of a line
  char* line=(char*)malloc(sizeof(char)*line_length);
  //sort ids in specific files
  if(opt & 8) printf("...... Sorting <%s>\r",filename);

  char ID[21];
  for(int i=0;i<id_num;i++){
    char ch=fgetc(fp);
    fsetpos(fp,&firstline);
    switch(i){
      case 0: strcpy(ID,ID0);
              break;
      case 1: strcpy(ID,ID1);
              break;
      case 2: strcpy(ID,ID2);
              break;
      case 3: strcpy(ID,ID3);
              break;
      case 4: strcpy(ID,ID4);
              break;
      case 5: strcpy(ID,ID5);
              break;
      case 6: strcpy(ID,ID6);
              break;
      case 7: strcpy(ID,ID7);
              break;
      default: fprintf(stderr,"\nError switch(i) line 62 in idsort.c\n"); //never happens
               exit(-45);
    }
    //filtering IDs in each tmp file
    while (ch!=EOF){
      fread(line, line_length+1, 1, fp);
      if(strstr(line,ID)!=NULL) fprintf(tmp[i],"%s",line);
      ch=fgetc(fp);
      fseek(fp, -1,SEEK_CUR);
    }
  }
  if(opt & 8){
    printf( BOLD WHT "[" GRN "done" WHT "]" RESET "\n");
    printf("...... Merging tmp files\r");
  }
  //merging ID files
  //cp first line

  char ln[95];
  //fgetpos(fp,&firstline);
  fseek(fp,0,SEEK_SET);
  fread(ln,95,1,fp);
  //fsetpos(fp,&firstline);
  FILE* tmp0=fopen("tmp0.csv","w");
  if(tmp0==NULL) perror("Error tmp0.csv: ");
  fprintf(tmp0,"%s",ln);
  for(int i=0;i<id_num;i++){
    switch(i){
      case 0: strcpy(ID,CID0);
              break;
      case 1: strcpy(ID,CID1);
              break;
      case 2: strcpy(ID,CID2);
              break;
      case 3: strcpy(ID,CID3);
              break;
      case 4: strcpy(ID,CID4);
              break;
      case 5: strcpy(ID,CID5);
              break;
      case 6: strcpy(ID,CID6);
              break;
      case 7: strcpy(ID,CID7);
              break;
      default: fprintf(stderr,"\nError switch(i) line 89 in idsort.c\n"); //never happens
               exit(-45);
    }//cp each file into tmp0.csv
    freopen("tmp0.csv","a",tmp0);
    fprintf(tmp0,"====ID:<%s>====\n",ID);
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
  if(opt & 8){
    printf( BOLD WHT "[" GRN "done" WHT "]" RESET "\n");
    printf("...... Removing tmp files\r");
  }
  //Removing files
  for(int i=0;i<id_num;i++){
    remove(File_deletion_list[i]);
    free(File_deletion_list[i]);
  }
  if(opt & 8) printf( BOLD WHT "[" GRN "done" WHT "]" RESET "\n");
  return 0;
}
