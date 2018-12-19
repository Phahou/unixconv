/* windows.c for converting windows CRLF into LF (dos into unix) */
//TODO: make dos.csv to a stdtmpfile
//Remove magic numbers

#include<stdio.h>

int rmwinCRLF(char* filename,USI* lineno_, USI* Max){
	USI lineno=0;
	USI c_in_line=0; //characters in line
	FILE *fp=fopen(filename,"r");

//handling openingfileerror from mentioned in config.c and idsort.c
	if (!fp){
		perror("");
		exit(-1);
	}

	FILE *tmp=fopen("dos.csv","w+");
	char c='0'; //char need to be signed if not EOF will never be reached!
	while(lineno<=52000){
		c=fgetc(fp);
		switch(c){
		case EOF:	goto EXIT;
		/* I mean sure I could also say lineno=99999, but imho this here is much
		 * clearer. Kind of like:"Oh we are done here? Ok let's get out!" */
		case '\r':	break; //do nothing
		case '\n':
			c_in_line++;
			if(c_in_line>*Max) *Max=c_in_line;
			lineno_[lineno]=c_in_line;
			lineno++;
			c_in_line=-1;	//reset
			/* fall through */
		default:
			c_in_line++;
			fprintf(tmp,"%c",c);
		}
	}
EXIT: //clean up
	fclose(tmp);
	fclose(fp);

	rename("dos.csv",filename);
	return 0;
}
