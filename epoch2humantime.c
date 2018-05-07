/* epoch2humantime.c for time convertion */

#include <time.h>

char* epoch2human(time_t now)
{ 
    struct tm  ts;       //needed for strftime which converts epoch(unix from machines) to humanreadable date and time
    static char buf[34]; //reserving space for output static bc I wanted it to be used by another func

    // Format time, "weekday yyyy-mm-dd hh:mm:ss timezone"
    ts = *localtime(&now);
    strftime(buf, sizeof(buf), "%a %Y-%m-%d %H:%M:%S %Z", &ts);
    return buf;
}
