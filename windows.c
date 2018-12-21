/* windows.c for converting windows CRLF into LF (dos into unix) */
//TODO: make dos.csv to a stdtmpfile
//Remove magic numbers

#include<stdio.h>

int rmwinCRLF_and_fill(char* filename, size_t* Max){
	USI curr_line_num=0; //current line number
	USI cil=0; //characters in line
	FILE *fp=fopen(filename,"r");

//handling opening-file-error from mentioned in config.c and idsort.c
	if (!fp){
		perror("");
		exit(-1);
	}

	FILE *tmp=fopen("dos.csv","w+");
	//EOF == -1 btw
	char c='0'; //char need to be signed if not EOF will never be reached!
	
	while(true){
		c=fgetc(fp);
		switch(c){
		case EOF:	goto EXIT;
		/* I mean sure I could also say break, but imho this here is much
		 * clearer. Kind of like:"Oh we are done here? Ok let's get out!" */
		case '\r':	break; //do nothing
		case '\n':
			cil++;
			if(cil>*Max) *Max=cil;
			curr_line_num++;
			cil=-1; //reset
			/* fall through */
		default:
			cil++;
			fprintf(tmp,"%c",c);
		}
	}
EXIT: //clean up
	fclose(tmp);
	fclose(fp);

	rename("dos.csv",filename);
	return 0;
}
