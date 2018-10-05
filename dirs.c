/*
 * this	program displays the names of all files in the current directory.
 */
#include <sys/stat.h> //dir recognition
#include <dirent.h> 
#include <unistd.h>
#include "list.c"
struct stat sb; //for checking if input is a dir

bool isdir(char* file);
list* checkdirs( char const * const path, list* files, char* pattern);
list* checkdirs_r(char* path, char* pattern); //calls checkdirs with extras

/*
 * dir: directory to search
 * sort: sort list with bubblesort?
 * pattern: file ending to search for
 * what it doesnt do: open the files it returns for sorting the false postives out
*/
list* dirs(char* dir, bool sort, char* pattern) {
    list* path=checkdirs_r(dir, pattern);
    if (sort) bubbleSort(path);
    return path;
}

/* dir functions */
bool isdir(char* file){
	if (stat(file, &sb) == 0 && S_ISDIR(sb.st_mode)){
		return true;
	}
	return false;
}

list* checkdirs_r(char* path, char* pattern){ //path is a folder
	int last=strlen(path)-1;
	if(path[last]=='/'){
		path[last]='\0';
	}
	if(isdir(path)){
		list* dir=(list*)calloc(sizeof(list),1);
		checkdirs(path,dir,pattern);
        list* p=dir->next;
        del_list(dir);
        return p;
	}
	return NULL;
}

/* list* checkdirs():
 * Searches 'path' recursively for files and
 * @returns a list of files which aren't folders
 */
list* checkdirs( char const * const path, list* files, char* pattern){
	DIR *d;
	struct dirent *dir;

	int s_path=strlen(path); //needed for malloc in while
	int s_dname=0, s_both=0;
	char *path_ptr;						 //malloc ptr
	long int i=0;
	d = opendir(path);
	list* curr_pos_list=files; //last elem which was added for increased performance
	if (d) {
		while ((dir = readdir(d)) != NULL) {
			i++;
			//skip dot and dot-dot folders otherwise it would start an infinitive loop
			if ((strcmp(dir->d_name,"..")==0)||(strcmp(dir->d_name,".")==0)) {
				continue;
			}
			s_dname=strlen(dir->d_name);
			s_both=s_path+s_dname+2;
			path_ptr=(char*)calloc(s_both,sizeof(char));

			sprintf(path_ptr,"%s/%s",path,dir->d_name);
			if(isdir(path_ptr)){	//check subfolders (relative path from startfolder)
				closedir(d);
				curr_pos_list = checkdirs(path_ptr, curr_pos_list, pattern);
				d = opendir(path);
				for(long int j=0;j<i;j++){
					dir=readdir(d);
				}
			}

			if (s_dname>4){ // file has at least 4 chars
				const char* file_format=&(dir->d_name[s_dname-4]);
				if (strcmp(pattern,file_format)==0){
					if(strstr(dir->d_name,DEF_FILENAME)){
						curr_pos_list=addlast(new_list(path_ptr),curr_pos_list);
					} else {
						free(path_ptr);
					}
				} else {
					free(path_ptr);
				}
			} else {
				free(path_ptr);
			}
		}
		closedir(d);
	}
	return curr_pos_list;
}
