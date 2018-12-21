/* config.c -> Custom Config for defining ID Names etc. */

/* Removing the magic numbers */
#define ENDPATTERN ".csv"       //File ending of the requested files usually ".csv"

/* Colors yay thanks to stackoverflow.com */
//TODO: needs porting to windows -> not done
// and should be removed from other source files -> done!
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


struct cfg_t{           //Default:
    size_t ml;          //maximal lines per device  1024
    size_t dn;          //installed devices         2
    char** id;          //IDs before
    char** cid;         //Custom IDs after
    char* pattern;      //usually "Timestamp"
    char* epoch;        //usually "Epochzeit"
    char* endpattern;   //usually .csv
    int tol;            //tolerance
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
    "dev_num=%u\n\n"
    "//Anfangsmuster einer noch nicht konvertiertem Dateo:\n"
    "Pattern=\"Timestamp\"\n"
    "//Anfangsmuster einer konvertierten Datei:\n"
    "Epoch=\"Epochzeit\"\n\n"
    "//Dateien nach denen gesucht wird:\n"
    "ENDPATTERN=.csv\n\n"
    "//Toleranz zum zusammenfuegen zwischen den Abfragen:\n"
    "tolerance=%u\n\n"
    "//Vorherige Gerätenamen:\n"
    "\"TIP-00078107-03-02\"\n"
    "\"TIP-00078155-03-02\"\n"
    "\n"
    "//Gewünschte Gerätenamen:\n"
    "\"ID A\"\n"
    "\"ID B\"\n", (unsigned int)1024,(unsigned int)2, (unsigned int) 120);
    fclose(cfg);
}

/* 
 * lnlen() aka line_length() 
 * returns: 'siz'+1 bytes until the next newline character is reached
 * or the NULL character is read so that '\n' can definetly be saved
 */
size_t lnlen(const char* str){
    size_t siz=0;
// if(str[siz]) is a null pointer check as a reminder
    while((str[siz]!='\n') && str[siz]) siz++;
    siz++;
    /* returns the whole size of the line instead of size-1
     * needed because the \n char should also be printed for simplicity
    */
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
    if(!cfg) {
        initcfg();
        cfg=fopen(CFG_PATH,"r");
    }

char* str=NULL;
{ //read the whole file into str
    fseek(cfg, 0, SEEK_END);
    size_t cfg_size = ftell(cfg);
    fseek(cfg, 0, SEEK_SET);
    str=(char*)calloc(cfg_size+1,sizeof(char));
    fread(str, cfg_size, 1, cfg);
}

    const char* str_id=strstr(str,"Vorherige Gerätenamen:\n")+strlen("Vorherige Gerätenamen:\n");

{ //get maximal lines per line (aml) & devices installed (adn)

//get size of alloc-call aml -> argument max line ... adn -> argument device numbers
    const char* aml=strstr(str,"max_line=")+strlen("max_line=");  //+strlen(xy) for 
    const char* adn=strstr(str,"dev_num=")+strlen("dev_num=");    //right char* pos(in array)
//remove the minus sign if it's there idk if it can still be negative then
    if(aml[0]=='-') aml=&aml[1];
    if(adn[0]=='-') adn=&adn[1];
    read_cfg->ml=atoi(aml); //Implies: that atoi stops number reading at '\n'
    read_cfg->dn=atoi(adn); //implicit: number stops at '\n'
}

{ //get epoch, pattern, endpattern & tolerance from config
/*
 * could be made into its own function when used with an array but imo it
 * is clearer like that than say:
 * "oh yeah cfg->patterns[1] == epoch which damn idiot doesn't know that?"
 *
 * So no this will stay here
 */

//pattern
    const char* pattern=strstr(str,"Pattern=")+strlen("Pattern=");
    size_t lnlength=lnlen(pattern);
    read_cfg->pattern=(char*)malloc( lnlength * sizeof(char) );
    memcpy( read_cfg->pattern , pattern , lnlength-1); //-1: don't copy the '\n'
    read_cfg->pattern[lnlength-1]='\0'; //terminate string

//epoch
    const char* epoch=strstr(str,"Epoch=")+strlen("Epoch=");
    lnlength=lnlen(epoch);
    read_cfg->epoch=(char*)malloc( lnlength * sizeof(char) );
    memcpy( read_cfg->epoch , epoch , lnlength-1); //-1: don't copy the '\n'
    read_cfg->epoch[lnlength-1]='\0';

//endpattern
    const char* endpattern=strstr(str,"ENDPATTERN=")+strlen("ENDPATTERN=");
    lnlength=lnlen(endpattern);
    read_cfg->endpattern=(char*)malloc( lnlength * sizeof(char) );
    memcpy( read_cfg->endpattern , endpattern , lnlength-1); //-1: don't copy the '\n'
    read_cfg->endpattern[lnlength-1]='\0';

//tolerance
    const char* tol=strstr(str,"tolerance=")+strlen("tolerance=");
    lnlength=lnlen(tol);
    char* atoitol=(char*)malloc( lnlength * sizeof(char) );
    memcpy( atoitol , tol , lnlength-1); //-1: don't copy the '\n'
    atoitol[lnlength-1]='\0';
    read_cfg->tol=atoi(atoitol);
    free(atoitol);
}

{ //get ID Names
    read_cfg->id=(char**)calloc(read_cfg->dn,sizeof(char*));

//it's just tokenization if you look close enough
    for(unsigned int i=0;i<read_cfg->dn;i++){ //+1 because invalid read and no \n
        size_t len=lnlen(str_id);
        read_cfg->id[i]=(char*)calloc(len,sizeof(char));
        //memcpy bc I don't want to corrupt str (with '\0') although strcpy
        memcpy(read_cfg->id[i], str_id, len-1);          // would be faster
        str_id=str_id+lnlen(str_id)+1;
    }
}

{ //get CID Names
    read_cfg->cid=(char**)calloc(read_cfg->dn,sizeof(char*));
    str_id=strstr(str,"Gewünschte Gerätenamen:\n")+strlen("Gewünschte Gerätenamen:\n");
    for(unsigned int i=0;i<read_cfg->dn;i++){ //+1 because invalid read and no \n
        size_t len=lnlen(str_id);
        read_cfg->cid[i]=(char*)calloc(len,sizeof(char));
        memcpy(read_cfg->cid[i], str_id, len-1);
        str_id=str_id+lnlen(str_id);
    }
}

//clean up
    free(str);
    if(cfg) fclose(cfg);
    return read_cfg;
}

struct cfg_t* loadcfg(void) {
    if ( access( CFG_PATH , F_OK )!= -1) {  //file exists
        return readcfg();
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
    if(cfg->id)         free(cfg->id);
    if(cfg->cid)        free(cfg->cid);
    if(cfg->pattern)    free(cfg->pattern);
    if(cfg->epoch)      free(cfg->epoch);
    if(cfg->endpattern) free(cfg->endpattern);
    free(cfg);
}
