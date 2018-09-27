 /* option.c for Program options bc it is nice to have options
	* -h for help
	* -v for verbose
	* -n CRLF line feeds to LF line feeds
	* -c append ;;;;; to file for better viewing in a csv software (eg libreoffice)
	* -r recursive option for folders (looks into folders and checks if there are any files in it)
	* Options are stored in one int: opt
	* bitfield: 000r vhnc
	*/

#define PATTERN "\"Timestamp\""
#define EPOCH "\"Epochzeit\""

//return 0 if no options were given and exits the program if the options inputed were wrong or not defined
int options(int argc,char** argv){
	int opt=0; //0000 0000 (least significant 8 bit bitfield)
	if (argc==1) { //skip function because we no options were given
		return opt;
	}
	for (int i=1;i<argc;i++){
		if(argv[i][0]=='-'){
			int len_argv=strlen(argv[i]);
			for(int j=1;j<len_argv;j++){
				switch(argv[i][j]){
					case 'r':	opt=opt | 16;//0001 0000
								break;
					case 'v':	opt=opt | 8; //0000 1000
								break;
					case 'h':	opt=opt | 4; //0000 0100
								break;
					case 'n':	opt=opt | 2; //0000 0010
								break;
					case 'c':	opt=opt | 1; //0000 0001
								break;
					default:	printf("Invalid Option: %s\n",argv[i]);
								exit(-1);
				}
			}
		}
	}
	return opt;
}

int alreadyconverted(const char* filename,FILE *fp, int opt){
	fpos_t pos;
	if(fp){
		fgetpos(fp,&pos);
	}
	char row[12]={ 0 }; //init with all 0
	fgets(row,12,fp);
	if(strcmp(PATTERN,row)==0){
		if(opt & 8) printf(BOLD BLU "\"%s\" isn't converted yet\n" RESET,filename);
		fsetpos(fp,&pos); //reseting fp
		return 1;

	} else if(strcmp(EPOCH,row)==0){
		printf("%s is already " BOLD GRN "converted" RESET "\n",filename);
		return 0;

	}
	printf(	"Patterns don't match\n"
			"Rename PATTERN(%s)in #define in options.c and compile again\n"
			"Or it could be that a folder was selected...\n",PATTERN);
	fsetpos(fp,&pos);
	return 2; // should normally never get here with the right input files
}
