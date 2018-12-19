/* ec.c */

/* Includes */
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <stdbool.h>


struct tm;
//TODO: remove static numbers
typedef struct ec_t {
    FILE *tmp;                                  //temporary filepointer
    char time[EPOCH_TIME_LENGTH];               //input time
    char* id;                                   //input ID (TIP...)
    const char* cid;                            //Custom ID
    char time_readable[CONVERTED_TIME_LENGTH];  //human readable time (converted)
    unsigned long value;                        //input value for calcdiff -> move to
    /* time_readable */
        void (*convertedTime)(struct ec_t *self);
        void (*printID)(struct ec_t *self, bool c_option_set, int id);
        void (*setID)(struct ec_t *self, const char* cid);
}ec;

void epoch2human(ec *self);
void printid_new(ec *self, bool c_option_set, int id);
void setid(ec* self, const char* cid);


ec* new_ec(const char* customid){
    ec* obj = (ec*)calloc(1,sizeof(ec));
    if(customid)    obj->cid=customid;
    obj->convertedTime = &epoch2human;
    obj->printID = &printid_new;
    obj->setID   = &setid;
    return obj;
}

void del_ec(ec* ecp){
    if(ecp) free(ecp);
}

/* epoch2human for time convertion */
void epoch2human(ec *self){
    time_t now = atoi(self->time);
    struct tm ts;
    ts = *localtime(&now);
    strftime(self->time_readable,
                sizeof(self->time_readable),
                "%Y-%m-%d %H:%M:%S", &ts);
}

void setid(ec* self, const char* cid){
    if(cid) self->cid=cid;
}

void printid_new(ec *self, bool c_option_set, int id){
    //ID names exchangeable with #define in options.c
    if(c_option_set){ //; wanted
        for (int i=0;i<id;i++){
            fprintf(self->tmp,";;;;;;");
        }
    }
    fprintf(self->tmp,"%s",self->cid);
}
