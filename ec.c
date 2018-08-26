/* ec.c */

/* Includes */
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include "config.c"
#include <stdbool.h>

struct tm;

typedef struct ec_t {
  FILE *tmp;                  //temporary filepointer
  char time[14];              //input time
  char id[20];                //input ID (TIP...)
  char CustomID[20];          //Custom ID
  char time_readable[34];     //human readable time (converted)
  unsigned long value;        //input value
  /* time_readable */
    void (*convertedTime)(struct ec_t *self);
    void (*printID)(struct ec_t *self, bool printtoterm, int opt);
}ec;

void epoch2human(ec *self);
void printid(ec *self, bool printtoterm, int opt);

ec* new_ec(char* customid){
  ec* obj = (ec*)calloc(1,sizeof(ec));
  if(customid!=NULL){
    strcpy(obj->CustomID,customid);
  }
  obj->convertedTime = &epoch2human;
  obj->printID = &printid;
  return obj;
}

void del_ec(ec* ecp){
  if(ecp!=NULL){
    free(ecp);
  }
}


/* epoch2human for time convertion */
void epoch2human(ec *self){
//  ec* self = (ec*)this;
  time_t now = atoi(self->time);
  struct tm  ts;
  ts = *localtime(&now);
  strftime(self->time_readable, sizeof(self->time_readable), "%a %Y-%m-%d %H:%M:%S %Z", &ts);
}

void printid(ec *self, bool printtoterm, int opt){
//  ec* self = (ec*)this;
  //ID names exchangeable with #define in options.c
  if(printtoterm==false){
    if(strcmp(self->id,ID0)==0){            //Device IDs from options.c
      strcpy(self->CustomID,CID0);
      fprintf(self->tmp,"%s",self->CustomID);     //Custom names
    }
    if(strcmp(self->id,ID1)==0){
      strcpy(self->CustomID,CID1);
      if (opt & 1) fprintf(self->tmp,";;;;;;");   //idiot-proof :P
      fprintf(self->tmp,"%s",self->CustomID);     //Wish of the customer
    }
    if(strcmp(self->id,ID2)==0){
      strcpy(self->CustomID,CID2);
      if (opt & 1) fprintf(self->tmp,";;;;;;;;;;;;");
      fprintf(self->tmp,"%s",self->CustomID);
    }
    if(strcmp(self->id,ID3)==0){
      strcpy(self->CustomID,CID3);
      if (opt & 1) fprintf(self->tmp,";;;;;;;;;;;;;;;;;;");
      fprintf(self->tmp,"%s",self->CustomID);
    }
    if(strcmp(self->id,ID4)==0){
      strcpy(self->CustomID,CID4);
      if (opt & 1) fprintf(self->tmp,";;;;;;;;;;;;;;;;;;;;;;;;");
      fprintf(self->tmp,"%s",self->CustomID);
    }
    if(strcmp(self->id,ID5)==0){
      strcpy(self->CustomID,CID5);
      if (opt & 1) fprintf(self->tmp,";;;;;;;;;;;;;;;;;;;;;;;;;;;;;;");
      fprintf(self->tmp,"%s",self->CustomID);
    }
    if(strcmp(self->id,ID6)==0){
      strcpy(self->CustomID,CID6);
      if (opt & 1) fprintf(self->tmp,";;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;");
      fprintf(self->tmp,"%s",self->CustomID);
    }
    if(strcmp(self->id,ID7)==0){
      strcpy(self->CustomID,CID7);
      if (opt & 1) fprintf(self->tmp,";;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;");
      fprintf(self->tmp,"%s",self->CustomID);
    }
  } else {
    //Device IDs from the options.c with custom names (CID)
	if(strcmp(self->id,ID0)==0) {
	  strcpy(self->CustomID,CID0);
	  printf("%s",CID0);
	}
	if(strcmp(self->id,ID1)==0) {
	  strcpy(self->CustomID,CID0);
	  printf("%s",CID0);
	}
	if(strcmp(self->id,ID2)==0) {
      strcpy(self->CustomID,CID0);
	  printf("%s",CID0);
	}
	if(strcmp(self->id,ID3)==0) {
	  strcpy(self->CustomID,CID0);
	  printf("%s",CID0);
	}
	if(strcmp(self->id,ID4)==0) {
	  strcpy(self->CustomID,CID0);
	  printf("%s",CID0);
	}
	if(strcmp(self->id,ID5)==0) {
	  strcpy(self->CustomID,CID0);
	  printf("%s",CID0);
	}
	if(strcmp(self->id,ID6)==0) {
	  strcpy(self->CustomID,CID0);
	  printf("%s",CID0);
	}
	if(strcmp(self->id,ID7)==0) {
	  strcpy(self->CustomID,CID0);
	  printf("%s",CID0);
	}

  }
}
