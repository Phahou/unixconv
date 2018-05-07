 /* option.c for Program options bc it is nice to have options
  * -h for help
  * -v for verbose
  * Options are stored in one int: opt
  * bitfield: 0000 vh00 
  */

#define PATTERN "\"Timestamp\""
#define EPOCH "\"Epochzeit\""

int options(int argc,const char** argv){
  int opt=0; //0000 0000
  for (int i=1;i<argc;i++){
    if(argv[i][0]=='-'){
      int len_argv=strlen(argv[i]);
      for(int j=1;j<len_argv;j++){
        if(argv[i][j]=='v') opt=opt | 8; //0000 1000
        else if(argv[i][j]=='h') opt=opt | 4; //0000 0100
        else {
          printf("Invalid Option: %s\n",argv[i]);
          exit(-1);
        }
      }
    }
  }
  return opt;
}

int alreadyconverted(const char* filename,FILE *fp, int opt){
  if(fp == NULL){
    perror(filename);
    return -1;
  }
  fpos_t pos;
  fgetpos(fp,&pos);
  char row[12];
  fgets(row,12,fp);
  if(strcmp(PATTERN,row)==0){
    //fclose(fp);
    if(opt & 8) printf(BOLD BLU "\"%s\" isn't converted yet\n" RESET,filename);
    fsetpos(fp,&pos); //reseting fp
    return 1;

  }else if(strcmp(EPOCH,row)==0){
    //fclose(fp);
    printf("%s is already ",filename);
    printf(BOLD GRN "converted\n" RESET);
    fsetpos(fp,&pos);
    return 0;
  }else printf("Patterns don't match\nRename PATTERN(%s) in #define in options.c and compile again\n",PATTERN);
  fsetpos(fp,&pos);
  return 1; // should normally never get here with the right input files
}
