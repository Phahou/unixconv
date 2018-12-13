/* config.c -> Custom Config for defining ID Names etc. */

/* Removing the magic numbers */
#define CONVERTED_TIME_LENGTH 50//max length of the converted time string
#define EPOCH_TIME_LENGTH 14  //The stringlength of the Epoch-input-time usually 14
#define ENDPATTERN ".csv"       //File ending of the requested files usually ".csv"

/* Colors yay thanks to stackoverflow.com */
//TODO: needs porting to windows and should be removed from other source files
#define RED   "\x1B[31m"
#define GRN   "\x1B[32m"
#define BLU   "\x1B[34m"
#define WHT   "\x1B[37m"
#define RESET "\x1B[0m"
#define BOLD  "\x1B[1m"

#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#define CFG_PATH "cfg.txt"


/* CFG
 * 0000 0001 -> ERR: new config was created
 * 0000 0010 -> ERR: Illegal configs
 */
char CFG=0; //FLAG variable for checking statuses of functions
#define CFG_NEW 1
#define CFG_ILL 2

struct cfg_t{   //        Default:
	size_t ml;  //maximal lines per device  1024
	size_t dn;  //installed devices   2
	char** id;  //IDs before
	char** cid; //Custom IDs after
};

/*
 * Creates a new config file and sets the
 * new config file created Flag to true
 */
void initcfg(void){
	FILE* cfg = fopen(CFG_PATH,"w");
	fprintf(cfg,
	"//Einstellungen:\n"
	"//Maximale Zeilen pro Datei:\n"
	"max_line=%u\n"
	"//Anzahl an Geräten: \n"
	"dev_num=%u\n"
	"//Vorherige Gerätenamen:\n"
	"'\"TIP-00078107-03-01\"'\n"
	"'\"TIP-00078155-03-02\"'\n"
	"\n"
	"//Gewünschte Gerätenamen:\n"
	"'\"ID A\"'\n"
	"'\"ID B\"'\n", (unsigned int)1024,(unsigned int)2);
	fclose(cfg);
	CFG=CFG | CFG_NEW;
}

/* 
 * lnlen() aka line_length() 
 * returns: 'siz' bytes until the next newline character is reached or the NULL character is read
 * '\0' increment isn't included in the return value
 */
size_t lnlen(const char* str){
	size_t siz=0;
	while((str[siz]!='\n') && str[siz]) siz++;
	//siz++; // returns the whole size of the line instead of size-1
	return siz;
}

/*
 * Reads the config file
 * returns a struct cfg_t containing all informations needed for the configuration
 * Sets the CFG_ILL Flag in CFG to 0 if successful
*/
struct cfg_t* readcfg(void) {
	struct cfg_t* read_cfg=(struct cfg_t*)calloc(1,sizeof(struct cfg_t));
	FILE* cfg= fopen(CFG_PATH,"r");

	//read the whole file
	fseek(cfg, 0, SEEK_END);
	size_t cfg_size = ftell(cfg);
	fseek(cfg, 0, SEEK_SET);
	char* str=(char*)calloc(sizeof(char),cfg_size+1);
	fread(str, cfg_size, 1, cfg);
	
	//get size of alloc-call aml -> argument max line ... adn -> argument device numbers
	const char* aml=strstr(str,"max_line=")+strlen("max_line=");  //+strlen(xy) for 
	const char* adn=strstr(str,"dev_num=")+strlen("dev_num=");    //right char* pos(in array)
	const char* str_id=strstr(str,"Vorherige Gerätenamen:\n")+strlen("Vorherige Gerätenamen:\n");
	read_cfg->ml=atoi(aml);
	read_cfg->dn=atoi(adn);
	
	//error handling
	if((read_cfg->ml<0) || (read_cfg->dn<0)) {
		CFG=CFG | CFG_ILL;
		return read_cfg;
	}
	
	//get ID Names
	read_cfg->id=(char**)calloc(sizeof(char*),read_cfg->dn);
	read_cfg->cid=(char**)calloc(sizeof(char*),read_cfg->dn);

	//it's just tokenization if you look close enough
	for(unsigned int i=0;i<read_cfg->dn;i++){
		read_cfg->id[i]=(char*)calloc(sizeof(char),lnlen(str_id));
		memcpy(read_cfg->id[i], str_id, lnlen(str_id));
		str_id=str_id+lnlen(str_id)+1;
	}

	//get CID Names
	str_id=strstr(str,"Gewünschte Gerätenamen:\n")+strlen("Gewünschte Gerätenamen:\n");
	for(unsigned int i=0;i<read_cfg->dn;i++){
		read_cfg->cid[i]=(char*)calloc(sizeof(char),lnlen(str_id));
		memcpy(read_cfg->cid[i], str_id, lnlen(str_id));
		str_id=str_id+lnlen(str_id)+1;
	}
	CFG = CFG & ~(CFG_ILL);  //Read successful
	return read_cfg;
}

struct cfg_t* loadcfg(void) {
	struct cfg_t* config;
	if ( access( CFG_PATH , F_OK )!= -1) {  //file exists
		config=readcfg();
		if( (CFG & CFG_ILL)==0 ){  //everything is fine
			return config;
		} else {  //illegal config
			fprintf(stderr,"Error: Config file is corrupted!\nCreating a new one\n");
			initcfg(); //create a new configs file
			return readcfg();
		}
	} else {  //file doesn't exist
		fprintf(stderr,"Error: Config file doesn't exist!\n");
		initcfg();
		return readcfg();
	}
}
void del_cfg(struct cfg_t* cfg){
	for(size_t i=0;i<cfg->dn;i++){
		if(cfg->id[i]) free(cfg->id[i]);
		if(cfg->cid[i]) free(cfg->cid[i]);
	}
	if(cfg->id) free(cfg->id);
	if(cfg->cid)  free(cfg->cid);
	free(cfg);
}
