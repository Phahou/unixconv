/* windows.c for converting windows CRLF into LF (dos into unix) */
#include<stdio.h>
int rmwinCRLF(int argc,const char** argv, int filesum){
  for (int i=1;i<=filesum;i++){
    if(argv[i][0]=='-') continue;                       //skipping options
    FILE *fp=fopen(argv[i],"r");
    FILE *tmp=fopen("dos.csv","w");
    while(1){
      char c=fgetc(fp);
      if (c==EOF){
        fclose(tmp);
        fclose(fp);
        break;

      } else if(c=='\r'){
        /* do nothing */
      } else {
        fprintf(tmp,"%c",c);
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