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
#include<sys/stat.h>

/* Own Includes: */
#include"epoch2humantime.c" /* for time name says it all :P             */
#include"options.c"         /* for setting eg -r for only reducing      */
#include"config.c"          /* Custom Config for defining ID Names etc. */
#include"idsort.c"          /* for sorting IDs in the right order       */
                            /* -> after ID1 is ID2 in the next line     */
                            /*    and so on                             */

struct stat sb;
int calc_diff(int row_a,FILE *fp,int opt);

//outsourcing functions
void printid(char* id,FILE* tmp,bool printtoterm){

  //ID names exchangeable with #define in options.c

  if(printtoterm==false){
    if(strcmp(id,ID0)==0){                  //Device IDs from options.c
      fprintf(tmp,"%s",CID0);               //Custom names
    }
    if(strcmp(id,ID1)==0){
      fprintf(tmp,";;;;;;");          //idiot-proof :P
      fprintf(tmp,"%s",CID1);         //Wish of the customer
    }
    if(strcmp(id,ID2)==0){
      fprintf(tmp,";;;;;;;;;;;;");
      fprintf(tmp,"%s",CID2);
    }
    if(strcmp(id,ID3)==0){
      fprintf(tmp,";;;;;;;;;;;;;;;;;;");
      fprintf(tmp,"%s",CID3);
    }
    if(strcmp(id,ID4)==0){
      fprintf(tmp,";;;;;;;;;;;;;;;;;;;;;;;;");
      fprintf(tmp,"%s",CID4);
    }
    if(strcmp(id,ID5)==0){
      fprintf(tmp,";;;;;;;;;;;;;;;;;;;;;;;;;;;;;;");
      fprintf(tmp,"%s",CID5);
    }
      if(strcmp(id,ID6)==0){
      fprintf(tmp,";;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;");
      fprintf(tmp,"%s",CID6);
    }
    if(strcmp(id,ID7)==0){
      fprintf(tmp,";;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;");
      fprintf(tmp,"%s",CID7);
    }

    } else {
      if(strcmp(id,ID0)==0){          //Device IDs from the options.c
        printf("%s",CID0);            //Custom names
      }
      if(strcmp(id,ID1)==0){
        printf("%s",CID1);
      }
      if(strcmp(id,ID2)==0){
        printf("%s",CID2);
      }
      if(strcmp(id,ID3)==0){
        printf("%s",CID3);
      }
      if(strcmp(id,ID4)==0){
        printf("%s",CID4);
      }
      if(strcmp(id,ID5)==0){
        printf("%s",CID5);
      }
      if(strcmp(id,ID6)==0){
        printf("%s",CID6);
      }
      if(strcmp(id,ID7)==0){
        printf("%s",CID7);
      }
    }
}
int idhasntchanged(char* id){
  return strncmp(id,"====",4);
}

int reduce2importantdata(const char* filename, int opt){
  FILE* fp;
  fp=fopen("tmp0.csv","r");
  int status=alreadyconverted(filename, fp, opt);
  if(status == 0) return 0;           //skip function nothing to do here
  else if(status == -1) return -1;    //Error while Opening stuff

  //okay the file isnt reduced yet...
  if(opt & 8) printf("...... Opening files for conversion\r");
  FILE *tmp=fopen("file.csv","w+");
  if(tmp == NULL){
    perror("\nError file.csv:");
    return -1;
  }
  if(opt & 8) printf(BOLD WHT "[" GRN "done" WHT "]" RESET "\n");

  fpos_t pos;
  fseek(fp,0,SEEK_SET);
  fprintf(tmp,"\"Epochzeit\";\"Zaehler-IDs\";\"Zaehlerwert\";\"Normale Zeit\";\"Stromdifferenz zwischen den Messungen\";\"Stromdifferenz ohne 0 & 1\"\n");
  unsigned int diff=0;
  if(opt & 8) printf("...... Reducing <%s>\r",filename);
  for(int asdl=0;asdl<=INSTALLED_IDS;asdl++){
    while(1){
      int i=0;
      fp=skiplines(fp,1);             //set fp to the beginning of the line
      fgetpos(fp,&pos);
      char isequal=fgetc(fp);
      fsetpos(fp,&pos);
      if(isequal=='='){               //print rest of line
        fgetpos(fp,&pos);
        while(fgetc(fp)!='\n') i++;
        char* idline=(char*)malloc(sizeof(char)*i);
        fsetpos(fp,&pos);
        fgets(idline,i,fp);
        fprintf(tmp,"%s\n",idline);
        free(idline);
        break;
      }

      char timestamp[12];             //Copy Time
      fgets(timestamp,12,fp);
      fprintf(tmp,"%s",timestamp);    //Timestamp done last letter from fgetc = ;

      char id[20];                    //Rewriting IDs
      fgets(id,21,fp);

      //compare IDs and print them in tmp file
      printid(id,tmp,false);

      fprintf(tmp,"%c",fgetc(fp));    //Copy Values
      fgetc(fp);                      //Without ""
      fgetpos(fp,&pos);
      i=0;
      for(i=0;fgetc(fp)!='"';i++) ;
      fsetpos(fp,&pos);
      
      char* values=(char*)malloc(sizeof(char)*i);
      for(int j=0;j<i;j++){
        values[j]=fgetc(fp);
      }
      
      fprintf(tmp,"%s",values);
      //preperations for calc_diff
      fgetc(fp); //"

      fprintf(tmp,"%c",fgetc(fp));
      fprintf(tmp,"\"%s\";",epoch2human(atoi(timestamp))); //Print time

      //calc_diff
      diff=calc_diff(atoi(values),fp,opt);
      if(diff==-10){
        if(opt & 8){
           printf("Reached End of Input for <");
          printid(id,tmp,true);
          printf(">\n");        
        }
        free(values);
        fflush(tmp);
        fflush(fp);
        break;
      }
      if(diff==-1){
        if(opt & 8){
          printf("Reached End of Input for <");
          printid(id,tmp,true);
          printf(">\n");
        }
        fprintf(tmp,"%u\n",0);
        free(values);
        fflush(tmp);
        fflush(fp);
        asdl--;
        break;
      }
      fprintf(tmp,"%u",diff);
      if((diff!=0)&&(diff!=1)) fprintf(tmp,";%u",diff);
      fprintf(tmp,"\n");
      free(values);
      fflush(tmp);
      fflush(fp);
    }
  }
  fclose(tmp);
  fclose(fp);
  rename("file.csv",filename);
  printf(BOLD WHT "[" GRN "done" WHT "]" RESET " %s\n",filename);
  return 0;
}
int calc_diff(int row_a,FILE *fp,int opt){
  fpos_t row_below,fp_reset;
  fgetpos(fp,&fp_reset);
  char ch='0';
  //getting the numbers from row_below
  while(ch!='\n'){ //skip line
    ch=fgetc(fp);
    if(ch==EOF) return -10;
  }

  for(int i=0;i<2;i++){               // going to 2nd ';' where the energy values start
    ch='0';
    while(ch!=';'){
      ch=fgetc(fp);
      if(ch=='=') {
        fsetpos(fp,&fp_reset);
        return -1; //next ID is here
      }
      if(ch==EOF) return -10;
    }
  }
  //done fp is now on "
  fgetc(fp); //let fp advance one char bc '"' - idk if fp++ would work EDIT: no it wouldnt
  fgetpos(fp,&row_below);
  
  //Copying Energy Values to string
  int i=0;
  for(i=0;ch!='"';i++){
    ch=fgetc(fp);
    if(ch==EOF) return -10;
  }
  char* energy_value=(char*)malloc(sizeof(char)*i);
  fsetpos(fp,&row_below);
  for(int j=0;j<(i-1);j++){
    energy_value[j]=fgetc(fp);
  }
  //atoi row_below
  int row_b=atoi(energy_value);
  free(energy_value);
  fsetpos(fp,&fp_reset);
  return row_b-row_a;
}

int main(int argc,const char** argv){
  int opt=options(argc,argv);
  unsigned int filesum=argc-1;                          //Number of files
  if((opt & 4)||(argc==1)){                             // help Message
    printf(BOLD WHT "Unixconv - A Epoch to human-readable converter 1.0\n\n" RESET);

    printf("Verwendung: unixconv [Argumente] [Datei ..]\n\n");

    printf("   Bitte Aufpassen: Die alten Dateien werden überschrieben, jedoch\n");
    printf("                    werden die Reste der Ursprungsdatei angehängt\n\n");

    printf("   Das Programm wird benutzt um die Rohdaten des MUCEasy(tm)\n");
    printf("   aufzubereiten & dabei zu verkleinern.\n\n");

    printf(BOLD WHT "Argumente:\n\n" RESET);

    printf("   -v    gesprächig(Es wird auf der Konsole ausgegeben\n");
    printf("         was gerade passiert)\n");
    printf("   -h    zeigt diese Hilfe an\n\n");
    return 0;
  }
  for(int i=1;i<=filesum;i++){
    if(argv[i][0]=='-') continue;                       //skipping options
    if (stat(argv[i], &sb) == 0 && S_ISDIR(sb.st_mode)){
      fprintf(stderr,"%s is a directory\n",argv[i]);
      continue;
    }
    if(!(opt & 1)) idsort(argv[i], INSTALLED_IDS, opt); //sort IDs in the right order
    reduce2importantdata(argv[i], opt);                 //main purpose of the program
  }
  if (opt & 8 ) printf("Cleaning up tmp0.csv\n");
  remove("tmp0.csv");
  return 0;
}

