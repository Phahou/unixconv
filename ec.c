/* ec.c */

/* Includes */
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include "config.c"
#include <stdbool.h>

struct tm;

typedef struct ec_t {
	FILE *tmp;									//temporary filepointer
	char time[EPOCH_TIME_LENGTH];				//input time
	char id[TIP_INPUT_LENGTH];					//input ID (TIP...)
	char CustomID[CUSTOMID_LENGTH];				//Custom ID
	char time_readable[CONVERTED_TIME_LENGTH];	//human readable time (converted)
	unsigned long value;						//input value for calcdiff -> move to
	/* time_readable */
		void (*convertedTime)(struct ec_t *self);
		void (*printID)(struct ec_t *self, bool c_option_set, int id);
		void (*setID)(struct ec_t *self, int id);
}ec;

void epoch2human(ec *self);
void printid_new(ec *self, bool c_option_set, int id);
void setid(ec* self, int id);


ec* new_ec(char* customid){
	ec* obj = (ec*)calloc(1,sizeof(ec));
	if(customid){
		strcpy(obj->CustomID,customid);
	}
	obj->convertedTime = &epoch2human;
	obj->printID = &printid_new;
	obj->setID   = &setid;
	return obj;
}

void del_ec(ec* ecp){
	if(ecp){
		free(ecp);
	}
}

/* epoch2human for time convertion */
void epoch2human(ec *self){
	time_t now = atoi(self->time);
	//printf("%s ->",self->time);
	struct tm ts;
	ts = *localtime(&now);
	strftime(self->time_readable,
			sizeof(self->time_readable),
          #ifdef __unix__
            "%Y-%m-%d %H:%M:%S", &ts);
          #endif // __unix__
          #ifdef __WIN32
            "%Y-%m-%d %H:%M:%S", &ts);
          #endif // __WIN32
    //printf(" %s\n", self->time_readable);
}

void setid(ec* self, int id){
	switch(id){
		case 0: strcpy(self->CustomID,CID0);
				break;
		case 1: strcpy(self->CustomID,CID1);
        		break;
		case 2: strcpy(self->CustomID,CID2);
        		break;
		case 3: strcpy(self->CustomID,CID3);
				break;
		case 4: strcpy(self->CustomID,CID4);
				break;
		case 5: strcpy(self->CustomID,CID5);
        		break;
		case 6: strcpy(self->CustomID,CID6);
        		break;
		case 7: strcpy(self->CustomID,CID7);
	}
}

void printid_new(ec *self, bool c_option_set, int id){
	//ID names exchangeable with #define in options.c
	if(c_option_set){ //; wanted
		switch(id) {
			case 0: fprintf(self->tmp,"%s",CID0);
					break;
			case 1: fprintf(self->tmp,";;;;;;%s",CID1);
					break;
			case 2: fprintf(self->tmp,";;;;;;;;;;;;%s",CID2);
					break;
			case 3: fprintf(self->tmp,";;;;;;;;;;;;;;;;;;%s",CID3);
					break;
			case 4: fprintf(self->tmp,";;;;;;;;;;;;;;;;;;;;;;;;%s",CID4);
					break;
			case 5: fprintf(self->tmp,";;;;;;;;;;;;;;;;;;;;;;;;;;;;;;%s",CID5);
					break;
			case 6: fprintf(self->tmp,";;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;%s",CID6);
					break;
			case 7: fprintf(self->tmp,";;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;%s",CID7);
		}
	} else {		//; not wanted
		switch(id) {
			case 0: fprintf(self->tmp,"%s",CID0);
					break;
			case 1: fprintf(self->tmp,"%s",CID1);
					break;
			case 2: fprintf(self->tmp,"%s",CID2);
					break;
			case 3: fprintf(self->tmp,"%s",CID3);
					break;
			case 4: fprintf(self->tmp,"%s",CID4);
					break;
			case 5: fprintf(self->tmp,"%s",CID5);
					break;
			case 6: fprintf(self->tmp,"%s",CID6);
					break;
			case 7: fprintf(self->tmp,"%s",CID7);
		}
	}
}
