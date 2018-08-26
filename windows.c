/* windows.c for converting windows CRLF into LF (dos into unix) */


/*TODO: make it for multiple files make it multithreaded */
#include<stdio.h>

int rmwinCRLF(const char* filename,unsigned int** lineno_, unsigned int* Max){
  static unsigned int k=0;
  unsigned int lineno=0;
  unsigned int c_in_line=0;
  FILE *fp=fopen(filename,"r");
  //handling openingfileerror from mentioned in config.c and idsort.c
  if (fp==NULL){
    fprintf(stderr,"%s: ",filename);
    perror("");
    exit(-1);
  }

  FILE *tmp=fopen("dos.csv","w+");
  char c='0'; //char need to be signed if not EOF will never be reached!
  while(1){
    c=fgetc(fp);
    if (  (c==EOF)||(lineno>=28000)  ){
      fclose(tmp);
      fclose(fp);
      break;
    } else if(c=='\r'){
      /* do nothing */
    } else if(c=='\n'){
      fprintf(tmp,"%c",c);
      if(c_in_line>*Max) *Max=c_in_line;
      lineno_[k][lineno]=c_in_line;
      lineno++; //line number
      //reset
      c_in_line=1;
    } else {
      c_in_line++;
      fprintf(tmp,"%c",c);
      fflush(tmp);
    }
  }
  rename("dos.csv",filename);
  k++;
  return 0;
}
