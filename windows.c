/* windows.c for converting windows CRLF into LF (dos into unix) */
#include<stdio.h>
int rmwinCRLF(int argc,const char** argv, int filesum,unsigned int* lineno_, unsigned int* Max){
	unsigned int lineno=0;
	unsigned int c_in_line=0;
	for (int i=1;i<argc;i++){
    if(argv[i][0]=='-') continue;                       //skipping options
    FILE *fp=fopen(argv[i],"r");
    FILE *tmp=fopen("dos.csv","w");
    char c='0'; //char need to be signed if not EOF will never be reached!
    while(1){
      c=fgetc(fp);
      if (c==EOF){
        fclose(tmp);
        fclose(fp);
        break;
      } else if(c=='\r'){
    	  /* do nothing */
      }else if (c=='\n'){
    	fprintf(tmp,"%c",c);
    	if(c_in_line>*Max) *Max=c_in_line;
    	lineno_[lineno]=c_in_line;
    	lineno++; //line number
    	//reset
    	c_in_line=1;
      } else {
    	c_in_line++;
        fprintf(tmp,"%c",c);
        fflush(tmp);
      }

    }
  rename("dos.csv",argv[i]);
  }
  return 0;
}
/*
int main(int argc,const char** argv){
  int filesum=argc-1;
  rmwinCRLF(argc,argv,filesum);
}*/
