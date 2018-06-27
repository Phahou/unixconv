/*
 * taken out of io.c because of OO in ec.c
 */
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

//calc_diff
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
