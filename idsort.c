/* idsort.c: Sorting files bc BUS Systems aren't perfect */

#include"Tmsort.c"
void* msort(void* th_);

int idsort(
char* filename, int opt,
unsigned short int* lineno_,
unsigned short int* highest ){ //id_nums: number of IDs
	ec* ec_0=new_ec(CID0);
	ln* ln_0=new_ln(ec_0);
	ln_0->fp=fopen(filename,"r");
	FILE* tmp[INSTALLED_IDS];

	if(opt & 8) printf(BOLD WHT "Generating tmp files...\n" RESET);

	//initializing tmp files
	for(int i=0;i<INSTALLED_IDS;i++){
		tmp[i]= tmpfile();
	}

	fpos_t firstline;
	ln_0->skipln(ln_0); //skip 1 lines
	fgetpos(ln_0->fp,&firstline);

	char* line=(char*)calloc(*highest,sizeof(char));
	//sort ids in specific files

	if(opt & 8) printf("...... Sorting <%s>",filename);

	pthread_t t_id[INSTALLED_IDS];
	//Create threads
	Tmst* threads[INSTALLED_IDS];
	for(int i=0;i<INSTALLED_IDS;i++){
		threads[i] = new_Threadedmsort_argv_t( lineno_,highest, filename, tmp[i],i,&firstline);
		pthread_create(&t_id[i],NULL ,msort, threads[i]);
	}

	//join threads
	for(int i=0;i<INSTALLED_IDS;i++){
		pthread_join(t_id[i], NULL);
		del_Threadedmsort_argv_t(threads[i]);// (TODO?)
	}


	//merging ID files
	//cp first line
	line=(char*)realloc(line,sizeof(char)*(lineno_[0]+2));
	fseek(ln_0->fp,0,SEEK_SET);
	fgets(line,lineno_[0]+2,ln_0->fp);

	//closing filename:
	fclose(ln_0->fp);

	FILE* tmp0=fopen("tmp0.csv","w");
	fprintf(tmp0,"%s",line);
	free(line);

	if(opt & 8){
		printf("\r" BOLD WHT "[" GRN "done" WHT "]" RESET "\n");
		printf("...... Merging tmp files");
	}

	for(int i=0;i<INSTALLED_IDS;i++){
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
		}
//cp each file into tmp0.csv
		freopen("tmp0.csv","a",tmp0);
		fprintf(tmp0,"====ID:<%s>====\n",ec_0->id);

		fseek(tmp[i], 0, SEEK_END);
		long unsigned tmpsize = ftell(tmp[i]);
		fseek(tmp[i], 0, SEEK_SET);
		char *string=(char*)calloc(sizeof(char),tmpsize+1);
		fread(string, tmpsize, 1, tmp[i]);
		fprintf(tmp0,"%s",string);

		fflush(tmp0);
		free(string);
	}
	free(ln_0);
	free(ec_0);
	for (unsigned short int i=0;i<INSTALLED_IDS;i++){
		fclose(tmp[i]);
	}

	fclose(tmp0);
	if(opt & 8){
		printf("\r" BOLD WHT "[" GRN "done" WHT "]" RESET "\n");
		printf("...... Removing tmp files");
	}

	if(opt & 8) printf("\r" BOLD WHT "[" GRN "done" WHT "]" RESET "\n");
	return 0;
}

void* msort(void* th_){
	Tmst* th=(Tmst*)th_;
	char* ch=NULL;
	unsigned short int lineno=1;
	fseek(th->ln_0->fp,th->lineno_[0]+1,SEEK_SET); //skips the first line +2 bc \n and first letter in newline
	switch(th->i){
		case 0: strcpy(th->ln_0->ecp->id,ID0);
				break;
		case 1: strcpy(th->ln_0->ecp->id,ID1);
				break;
		case 2: strcpy(th->ln_0->ecp->id,ID2);
				break;
		case 3: strcpy(th->ln_0->ecp->id,ID3);
				break;
		case 4: strcpy(th->ln_0->ecp->id,ID4);
				break;
		case 5: strcpy(th->ln_0->ecp->id,ID5);
				break;
		case 6: strcpy(th->ln_0->ecp->id,ID6);
				break;
		case 7: strcpy(th->ln_0->ecp->id,ID7);
				break;
	}

	do{
		ch=fgets(th->line, th->lineno_[lineno]+2, th->ln_0->fp);
		if(strstr(th->line,th->ln_0->ecp->id)) {
			fprintf(th->ln_0->ecp->tmp,"%s",th->line);
		}
		lineno++;
	} while(ch);

	fflush(th->ln_0->ecp->tmp);
	pthread_exit(0);
}
