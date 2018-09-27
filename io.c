/* io.c for I/O Operations. Is at the same time the "main" compiling file */

/* ToC
 * 1. Includes
 * -> POSIX libs
 * -> Own Includes (with OO Concepts)
 * 2. function definitions
 * 3. main function
 * 4. Helper functions
 */

/*TODO:
 * change the reading to read one line and use it in the whole loop instead of moving the pointer always forward
 * remove magic numbers
 */

/* 1. Includes */
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<stdbool.h>
#include<sys/stat.h>/* needed for recognizing if input is a dir */
#include<pthread.h> /* for multithreading in idsort()			*/

/* Own Includes: */
#include"line.c"	/* data-structures holding important infos	*/
#include"options.c" /* for setting eg -r for only reducing		*/
#include"config.c"	/* Custom Config for defining ID Names etc. */
#include"idsort.c"	/* for sorting IDs in the right order		*/
					/* ->after ID1 is ID2 in the next line		*/
					/*	 and so on								*/
#include"windows.c"	/* converts windows CRLF into unix LF		*/
#include"dirs.c"	/* for checking folders recursively			*/

struct stat sb; //for checking if input is a dir

/* 2. function definitions */
int idhasntchanged(char* id);
int reduce2importantdata(const char* filename, int opt);
int isequalcheck(ln* lpr, fpos_t *pos);
int fileinit(ln* lpr, int opt);
void reachedEOF(ln* lpr, int status, int opt);
void printfirstline(ln* lpr);
bool printhelp(int opt, int argc);

/* 3. main function */

int main(int argc,char** argv){
	int opt=options(argc,argv);

	if (printhelp(opt, argc)){
		fprintf(stderr, "Zu wenig Argumente!\n");
		exit(0);	// help Message
	}

	unsigned short int MaxCharsLine=0, inputed_files=0;
	unsigned short int lineno_[MAX_LINE_NUMBERS_PER_DEVICE*INSTALLED_IDS]= { 0 };
	int k=0; //variable for checking if sth went acording to plan

	for (int i=1;i<argc;i++){ //check if file exists and TODO: isnt random garbage
		if (argv[i][0]!='-') inputed_files++;
	}
	if(inputed_files==0){
		fprintf(stderr,"Keine Dateien ausgewählt!\n");
		return 0;
	}


	for(int i=1;i<argc;i++){
		if(argv[i][0]=='-') continue;	//skipping options bug
		if(argc<=k) break;
		else k++; 
		/* 
		 * checks if the parameter is a directory
		 *	 returns a *list* of .csv-files in the directory if so
		 *	 returns *NULL* if not
		 * files in subdirectories will be added to the list
		 */
		if(isdir(argv[i])) {
			continue;
			/*if(opt & 32){
				files=dirs(argv[i]);
			}
			else if(opt & 8) 
					printf("Skipping %s: Directory\n",argv[i]);*/
		}

		//check if it is already converted 1 = no 0 = yes
		FILE* fp=fopen(argv[i],"r");
		int not_converted=alreadyconverted(argv[i], fp, opt);
		fclose(fp);
		

		if(not_converted==2) {
			fprintf(stderr,"Something seems wrong with this file: %s\nI will skip it.",argv[i]);
			continue;
		}
		if(not_converted){
			//converting windows CRLF into unix LF
			if (opt & 2){
				rmwinCRLF(argv[i], lineno_, &MaxCharsLine);
			}
			//sort IDs in the right order
			idsort(argv[i], opt, lineno_, &MaxCharsLine);
			//main purpose of the program
			int status=reduce2importantdata(argv[i], opt);
			if (status==-1) i--; //try again
		}
		if ( (opt & 8) && (NULL != fopen("tmp0.csv","r")) ){
			printf("Cleaning up tmp0.csv\n");
			remove("tmp0.csv");
		}
	}
	return 0;
}


int idhasntchanged(char* id){
	return strncmp(id,"====",4);
}

int reduce2importantdata(const char* filename, int opt){
	ln* lp =new_ln(new_ec(CID0));	//line-pointer
	lp->fp=fopen("tmp0.csv","r");
	if(!lp->fp){		//Error while Opening stuff
		del_ln(lp);
		return -1;
	}

	//okay the file isnt reduced yet...
	if(fileinit(lp,opt)==-1) return -1;
	//tmp file is open now
	fpos_t pos;
	fseek(lp->fp,0,SEEK_SET);
	printfirstline(lp);

	lp->diff=0;
	int status;
	if(opt & 8) printf("Reducing <%s>\n",filename);
	for(int done_ids=0;done_ids<=INSTALLED_IDS;done_ids++){
		while(1){
			status=isequalcheck(lp, &pos);
			if(status==-1) break;
			else if (status==-2) lp->skipln(lp);

//get data:
			fgets(lp->ecp->time,11,lp->fp);
			fprintf(lp->ecp->tmp,"%s",lp->ecp->time);
			fprintf(lp->ecp->tmp,"%c",fgetc(lp->fp) );

//Rewriting IDs
			fgets(lp->ecp->id,21,lp->fp);	//id

//compare IDs and print them in tmp file
			lp->ecp->printID(lp->ecp, opt & 1, done_ids-1);
			if(opt & 8) lp->ecp->setID(lp->ecp,done_ids-1);
			fprintf(lp->ecp->tmp,"%c",fgetc(lp->fp));	//Copy Values

			fgetc(lp->fp);	//Without ""
			fgetpos(lp->fp,&pos);
			int i=0;
			char ch='0';
			for(i=0; ch!='"' && ch!=EOF;i++) ch=fgetc(lp->fp);
			fsetpos(lp->fp,&pos);

//data conversion -> get value & print value
			char* values=(char*)calloc(sizeof(char),i);
			for(int j=0;j<i;j++){
				values[j]=fgetc(lp->fp);
			}
			lp->ecp->value=strtoul(values,NULL,10);
			free(values);
			fprintf(lp->ecp->tmp,"%lu;",lp->ecp->value);
		
//check if fp is on the right pos
			fgetc(lp->fp); //skip "
			fgetc(lp->fp); //skip ;

//time
			lp->ecp->convertedTime(lp->ecp);
			fprintf(lp->ecp->tmp,"\"%s\";",lp->ecp->time_readable); //Print time

//calc_diff
			int calc_status=lp->calc_diff(lp->ecp->value, lp);

			if(calc_status==-10){
				reachedEOF(lp, calc_status, opt);
				break;
			}
			if(calc_status==-1){
				reachedEOF(lp, calc_status, opt);
				done_ids--;
				break;
			}

			fprintf(lp->ecp->tmp,"%lu",lp->diff);
			
			if( (lp->diff != 0) &&
				(lp->diff != 1) ) fprintf(lp->ecp->tmp,";%lu",lp->diff);

			fprintf(lp->ecp->tmp,"\n");
//save line
			fflush(lp->ecp->tmp);
			fflush(lp->fp);
		}
	}
	fclose(lp->ecp->tmp);
	fclose(lp->fp);
	del_ln(lp);
	rename("file.csv",filename);
	remove("tmp0.csv");
	printf(BOLD WHT "[" GRN "done" WHT "]" RESET " %s\n",filename);
	return 0;
}

/* 4. Helper functions */
int isequalcheck(ln* lpr, fpos_t *pos){
	int i=0;
	lpr->skipln(lpr);

	fgetpos(lpr->fp,pos);
	char isequal=fgetc(lpr->fp);
	if(isequal=='='){	//print rest of line e.g. ===ID: A===
		fsetpos(lpr->fp,pos);
		i=lpr->getlnlen(lpr);
		fsetpos(lpr->fp,pos);
		char* idline=(char*)malloc(sizeof(char)*i);
		fgets(idline,i,lpr->fp);
		fprintf(lpr->ecp->tmp,"%s\n",idline);
		free(idline);
		return -1;
	} else if (isequal=='\n') return -2;
	fsetpos(lpr->fp,pos);
	return 0;
}

int fileinit(ln* lpr, int opt){
	if(opt & 8) printf("...... Opening files for conversion\r");
	lpr->ecp->tmp=fopen("file.csv","w+");
	if(!lpr->ecp->tmp){
		perror("\nError file.csv:");
		return -1;
	}
	if(opt & 8) printf(BOLD WHT "[" GRN "done" WHT "]" RESET "\n");
	return 0;
}

void reachedEOF(ln* lpr, int status, int opt){
	if(opt & 8){
		printf("Reached End of Input for <%s>\n",lpr->ecp->CustomID);
	}
	//printing a 0 for diff bc we have only 1 value
	if(status==-1) fprintf(lpr->ecp->tmp,"0\n");
	fflush(lpr->ecp->tmp);
	fflush(lpr->fp);
}

void printfirstline(ln* lpr){
	fprintf(lpr->ecp->tmp,"\"Epochzeit\";\"Zaehler-IDs\";\"Zaehlerwert\";");
	fprintf(lpr->ecp->tmp,"\"Normale Zeit\";\"Stromdifferenz zwischen den ");
	fprintf(lpr->ecp->tmp,"Messungen\";\"Stromdifferenz ohne 0 & 1\"\n");
	fflush(lpr->ecp->tmp);
}

bool printhelp(int opt, int argc){
	bool printed=false;
	if((opt & 4)||(argc==1)){
		printed=true;
		printf(BOLD WHT
		"Unixconv - A Epoch to human-readable converter 1.1\n\n" RESET

		"Verwendung: unixconv [Argumente] [Datei ..]\n\n"

		"    Bitte Aufpassen: Die alten Dateien werden überschrieben.\n\n"

		"    Das Programm wird benutzt um die Rohdaten des MUCEasy™\n"
		"    aufzubereiten & dabei zu verkleinern.\n\n" BOLD WHT

		"Argumente:\n\n" RESET

		"    -v  gesprächig (Es wird auf der Konsole ausgegeben was gerade passiert)\n"
		"    -h  zeigt diese Hilfe an\n\n"

		"    -n  wandelt CRLF-Zeilenumbrüche(DOS) in LF Zeilenumbrüche(Unix) um.\n"
		"        Diese Option wird gebraucht, wenn DOS-Dateien vorliegen. Es ist\n"
		"        eine gute Idee es allgemein mitzuübergeben.\n\n"

		"    -c  sorgt dafür das in der Ausgabedatei mehrere Semikolons (;) sind\n"
		"        um eine bessere Ansicht in Office Programmen zu haben wie bspw.\n"
		"        Libreoffice. (Sinnvoll, falls man mit den Daten direkt arbeiten\n"
		"        will, statt sie weiterverarbeiten zu lassen.\n\n");

	}
	return printed;
}
