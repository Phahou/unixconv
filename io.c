/* io.c for I/O Operations. Is at the same time the "main" compiling file */

/* Colors yay thanks to stackoverflow.com */
#define RED   "\x1B[31m"
#define GRN   "\x1B[32m"
#define BLU   "\x1B[34m"
#define WHT   "\x1B[37m"
#define RESET "\x1B[0m"
#define BOLD  "\x1B[1m"

/* Includes */
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<stdbool.h>
#include<sys/stat.h>  /* needed for recognizing if input is a dir */
#include<pthread.h>   /* for multithreading in idsort()           */

/* Own Includes: */
#include"line.c"      /* data-structures holding important infos  */
#include"options.c"   /* for setting eg -r for only reducing      */
#include"config.c"    /* Custom Config for defining ID Names etc. */
#include"idsort.c"    /* for sorting IDs in the right order       */
                      /* -> after ID1 is ID2 in the next line     */
                      /*    and so on                             */
#include"windows.c"   /* converts windows CRLF into unix LF       */


struct stat sb; //for checking if input is a dir
void printhelp();
void printfirstline(ln* lpr);
int fileinit(ln* lpr, int opt);
int isequalcheck(ln* lpr, fpos_t *pos);
void reachedEOF(ln* lpr, int status, int opt);


int idhasntchanged(char* id){
  return strncmp(id,"====",4);
}

int reduce2importantdata(const char* filename, int opt){
  ec* ecp=new_ec(CID0); //energy-counter-pointer
  ln* lp =new_ln(ecp);  //line-pointer
  lp->fp=fopen("tmp0.csv","r");
  int status=alreadyconverted(filename, lp->fp, opt);
  if(status == 0) return 0;           //skip function nothing to do here
  else if(status == -1) return -1;    //Error while Opening stuff

  //okay the file isnt reduced yet...
  if(fileinit(lp,opt)==-1) return -1;
  //tmp file is open now
  fpos_t pos;

  fseek(lp->fp,0,SEEK_SET);
  printfirstline(lp);

  fflush(lp->ecp->tmp);
  fflush(lp->fp);

  lp->diff=0;
  if(opt & 8) printf("...... Reducing <%s>\r",filename);
  for(int done_ids=0;done_ids<=INSTALLED_IDS;done_ids++){
    while(1){
      status=isequalcheck(lp, &pos);
      if(status==-1) break;
      else if (status==-2) lp->skipln(lp);

    //get data:
      fgets(lp->ecp->time,12,lp->fp);
      fprintf(ecp->tmp,"%s",ecp->time);  //epoch done

    //Rewriting IDs
      fgets(lp->ecp->id,21,lp->fp); //>implying the IDs are all 21 bytes long

//compare IDs and print them in tmp file
      lp->ecp->printID(lp->ecp,false);

      fprintf(ecp->tmp,"%c",fgetc(lp->fp));     //Copy Values

      fgetc(lp->fp);                            //Without ""
      fgetpos(lp->fp,&pos);
      int i=0;
      for(i=0;fgetc(lp->fp)!='"';i++) ;
      fsetpos(lp->fp,&pos);
//data conversion -> get value & print value
      char* values=(char*)malloc(sizeof(char)*i);
      for(int j=0;j<i;j++){
        values[j]=fgetc(lp->fp);
      }
      lp->ecp->value=strtoul(values,NULL,10);
      free(values);
      fprintf(lp->ecp->tmp,"%lu",ecp->value);

    //check if fp is on the right pos
      fgetc(lp->fp); //"
      char ch='0';
      ch=fgetc(lp->fp);
      if(ch==';') fprintf(ecp->tmp,"%c",ch);
      else perror("File will be corrupted...");

//time
      lp->ecp->convertedTime(ecp);
      fprintf(ecp->tmp,"\"%s\";",lp->ecp->time_readable); //Print time

//calc_diff
      int calc_status=lp->calc_diff(lp->ecp->value, lp/*, opt*/);
      if(calc_status==-10){
        reachedEOF(lp, calc_status, opt);
        break;
      }
      if(calc_status==-1){
        reachedEOF(lp, calc_status, opt);
        done_ids--;                     /* ???????????? */
        break;
      }
      fprintf(lp->ecp->tmp,"%lu",lp->diff);
      if((lp->diff!=0)&&(lp->diff!=1)) fprintf(ecp->tmp,";%lu",lp->diff);
      fprintf(lp->ecp->tmp,"\n");
    //save
      fflush(lp->ecp->tmp);
      fflush(lp->fp);
    }
  }
  fclose(lp->ecp->tmp);
  fclose(lp->fp);
  free(lp);
  free(ecp);
  //rename("file.csv",filename);
  printf(BOLD WHT "[" GRN "done" WHT "]" RESET " %s\n",filename);
  return 0;
}

int main(int argc,const char** argv){
  unsigned int MaxCharsLine=0;
  unsigned int lineno_[4096];	//saves the letters in a line
  	  	  	  	  	  	  	  	//-> I dont think a file will have that much lines

  //TODO: make lineno_ 2dimensional so every file has an array
  int opt=options(argc,argv);
  unsigned int filesum=0;       //Number of files
  for (int i=1;i<argc;i++){
	  if (!(argv[i][0]=='-')) filesum++;
  }
  printhelp(opt, argc);			// help Message

  if (opt & 2){                 //converting windows CRLF into unix LF
    rmwinCRLF(argc,argv,filesum, lineno_,&MaxCharsLine);
  }

  for(unsigned int i=1;i<=filesum;i++){
    if(argv[i][0]=='-') continue;  //skipping options

    //check if input is dir
    if (stat(argv[i], &sb) == 0 && S_ISDIR(sb.st_mode)){
      fprintf(stderr,"%s is a directory\n",argv[i]);
      continue;
    }

    if(!(opt & 1)) idsort(argv[i], opt, lineno_, &MaxCharsLine); //sort IDs in the right order
    reduce2importantdata(argv[i], opt);                 //main purpose of the program
  }
  if (opt & 8 ) printf("Cleaning up tmp0.csv\n");
  remove("tmp0.csv");
  return 0;
}

















/* Helper functions */

int isequalcheck(ln* lpr, fpos_t *pos){
  int i=0;
  lpr->skipln(lpr);

  fgetpos(lpr->fp,pos);
  char isequal=fgetc(lpr->fp);
  if(isequal=='='){             //print rest of line e.g. ===ID: A===
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
  if(lpr->ecp->tmp == NULL){
    perror("\nError file.csv:");
    return -1;
  }
  if(opt & 8) printf(BOLD WHT "[" GRN "done" WHT "]" RESET "\n");
  return 0;
}

void reachedEOF(ln* lpr, int status, int opt){
  if(opt & 8){
    printf("Reached End of Input for <");
    lpr->ecp->printID(lpr->ecp,true);
    printf(">\n");
  }
  if(status==-1) fprintf(lpr->ecp->tmp,"%u\n",0);
  fflush(lpr->ecp->tmp);
  fflush(lpr->fp);
}

void printfirstline(ln* lpr){
  fprintf(lpr->ecp->tmp,"\"Epochzeit\";\"Zaehler-IDs\";\"Zaehlerwert\";");
  fprintf(lpr->ecp->tmp,"\"Normale Zeit\";\"Stromdifferenz zwischen den ");
  fprintf(lpr->ecp->tmp,"Messungen\";\"Stromdifferenz ohne 0 & 1\"\n");
}

void printhelp(int opt, int argc){
  if((opt & 4)||(argc==1)){
  printf(BOLD WHT "Unixconv - A Epoch to human-readable converter 1.0\n\n" RESET);
  	  	  /* empty line */
  printf("Verwendung: unixconv [Argumente] [Datei ..]\n\n"
		  /* empty line */
		 "  Bitte Aufpassen: Die alten Dateien werden überschrieben, jedoch\n"
		 "                   werden die Reste der Ursprungsdatei angehängt.(fixed)™\n\n");

  printf("  Das Programm wird benutzt um die Rohdaten des MUCEasy™\n"
		 "  aufzubereiten & dabei zu verkleinern.\n\n");

  printf(BOLD WHT "Argumente:\n\n" RESET);

  printf("  -v   gesprächig (Es wird auf der Konsole ausgegeben was gerade passiert)\n"
		 "  -h   zeigt diese Hilfe an\n\n"
		  /* empty line */
		 "  -n   wandelt CRLF-Zeilenumbrüche(DOS) in LF Zeilenumbrüche(unix),\n"
		 "       diese Option wird gebraucht, wenn DOS-Dateien vorliegen, da.\n");
  }
}
