/* ec.c */

/* Includes */
#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <stdbool.h>


struct tm;

typedef struct ec_t {
    FILE* tmp;                  //temporary filepointer
    const char* time;           //input-time-pointer (for strchr)
    char time_readable[32];     /* human readable time (converted)
                                 * 32 bytes should be more than enough */
}ec;

void epoch2human(ec *self);

ec* new_ec(void){
    ec* obj = (ec*)calloc(1,sizeof(ec));
    return obj;
}

void del_ec(ec* ecp){
    if(ecp) {
        if(ecp->tmp) fclose(ecp->tmp);
        free(ecp);
    }
}

/* epoch2human for time convertion (not used)*/
void epoch2human(ec *self){
    time_t now = atoi(self->time);
    struct tm ts;
    ts = *localtime(&now);
    strftime(self->time_readable,
                sizeof(self->time_readable),
                "%Y-%m-%d %H:%M:%S", &ts);
}
