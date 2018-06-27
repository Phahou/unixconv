#include "line.c"

void main(){

  ec* Energy=new_ec("Labor 1");
  ec* vl=new_ec("value 1");
  ln* Line=new_ln(Energy);
  strcpy(Energy->time,"1928642021732");
  Energy->convertedTime(Energy);
  printf("%s\n",Energy->time_readable);
  
}
