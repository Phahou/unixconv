/* windows.c for converting windows CRLF into LF (dos into unix) */


/*TODO: make it for multiple files make it multithreaded */
#include<stdio.h>

int rmwinCRLF(char* filename,unsigned short int* lineno_, unsigned short int* Max){
	unsigned short int lineno=0;
	unsigned short int c_in_line=0; //characters in line
	FILE *fp=fopen(filename,"r");
	//handling openingfileerror from mentioned in config.c and idsort.c
	if (!fp){
		fprintf(stderr,"%s:lal ",filename);
		perror("");
		exit(-1);
	}

	FILE *tmp=fopen("dos.csv","w+");
	char c='0'; //char need to be signed if not EOF will never be reached!
	while(1){
		c=fgetc(fp);
		if ( (c==EOF)||(lineno>=28000) ){
			fclose(tmp);
			fclose(fp);
			break;
		} else if(c=='\r'){
			/* do nothing */
		} else if(c=='\n'){
			fprintf(tmp,"%c",c);
			if(c_in_line>*Max) *Max=c_in_line;
			lineno_[lineno]=c_in_line;
			lineno++; //line number
			//reset
			c_in_line=1;
		} else {
			c_in_line++;
			fprintf(tmp,"%c",c);
		}
	}
	rename("dos.csv",filename);
	return 0;
}
