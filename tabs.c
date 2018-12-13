#include<stdio.h>
#include<stdbool.h>
#include<string.h>
#include<stdlib.h>
//TODO: Buggy code might have to rework this
void tabs(
const char* filename,
unsigned int dn,
unsigned int ml, //unused
int tol){
	FILE* tmp=fopen("out.csv","w+");
	fputs("time\t\t",tmp);
	for(unsigned short int i=1;i<dn*2;i++){
		fprintf(tmp,"%d\t%dDIFF\t",i,i);
	}
	fputc('\n',tmp);

//read complete file into str
	FILE* fp=fopen(filename,"r+");
	fseek(fp, -1, SEEK_END);

//put \n on end if needed
	if(fgetc(fp)!='\n') fputc('\n',fp);
	fflush(fp);
	fseek(fp, 0, SEEK_END);
	size_t fp_size= ftell(fp);
	fseek(fp, 0, SEEK_SET);
	char* str=(char*)calloc(fp_size+40,sizeof(char));
	str[fp_size]='\0';
	fread(str, fp_size, 1, fp);
	fclose(fp);

//init pointers
	char** time=(char**)calloc(dn,sizeof(char*));
	char** id_vals=(char**)calloc(dn,sizeof(char*));
	char** id_diff=(char**)calloc(dn,sizeof(char*));
	char** cpy=(char**)calloc(dn,sizeof(char*));

//set pointers
	for(unsigned int i=0;i<dn;i++){
		cpy[i]=strstr(str,"====ID:<");
		cpy[i][0]='a';
		cpy[i]=strchr(cpy[i],'\n');
		if(cpy[i]){
			if(cpy[i][1]) cpy[i]=time[i]=&cpy[i][1];
		}
	}

//get lines from ID A to ID B (Assuming every device has the same number of lines)
	unsigned int x=0;
	char* y=str;
	y=strchr(y,'\n')+1;
	y=strchr(y,'\n');
	while(y[1]!='a') {
		y=strchr(y+1,'\n');
		x++;
	}
	char nullval[]="0";
	char* nlortk=NULL;
	char* lasttime;
	char* lastid_val_change;
	bool lp=false;
	for(unsigned int k=0;k<x;k++){		//WOHOO POINTERS!!11
		for(unsigned int i=0;i<dn;i++){ //No but srsly it is somewhat weird and kinda dirty imo
			cpy[i]=strchr(time[i],';'); //Note: strtok is garbage for iteration work so I had to do it this way
			if(cpy[i]) {
				cpy[i][0]='\0';
				lastid_val_change=id_vals[i]=cpy[i]=&cpy[i][1];
				cpy[i]=strchr(id_vals[i],';');
				cpy[i][0]='\0';
				cpy[i]=strchr(cpy[i]+1,';');
				id_diff[i]=cpy[i]=&cpy[i][1];
				nlortk=strchr(id_diff[i],';');
				cpy[i]=strchr(cpy[i]+1,'\n');
				if(cpy[i]){
					if(((nlortk)&&(nlortk-cpy[i])<0))	nlortk[0]='\0';
					cpy[i][0]='\0';
					if(cpy[i][1])	cpy[i]=&cpy[i][1];
					else {
						fflush(tmp);
						if(i==(dn-1)) {	//fucking get out of here
							lp=true;
							goto LASTPRINT;
						} else {
							goto EXIT;
						}
					}
				}
			} else {
				id_vals[i]=lastid_val_change;
				id_diff[i]=nullval; //last call
				time[i]=lasttime;
			}

		//print and check
			if(i==(dn-1)){
				LASTPRINT:
				if(atoi(id_diff[i])>2) {
					fprintf(tmp,"%s",time[0]);
					for(unsigned int j=0;j<dn;j++){
						if(     ((atoi(time[0])+tol) >= atoi(time[j])) &&
								((atoi(time[0])-tol) <= atoi(time[j]))     ){
							fprintf(tmp,"\t%s\t%s",id_vals[j],id_diff[j]);
							fflush(tmp);
						}
					}
					fputc('\n',tmp);
					fflush(tmp);
				}
				if(lp) goto EXIT;
				for(unsigned int l=0;l<dn;l++) time[l]=&cpy[l][0];
			}
			if(cpy[i]) ;
			else goto EXIT;
		}
	}
//clean up
EXIT:
	free(id_vals);
	free(id_diff);
	free(time);
	free(str);
	free(cpy);
	fflush(tmp);
	fclose(tmp);
}
