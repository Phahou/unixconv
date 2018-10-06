/*
 * this	program displays the names of all files in the current directory.
 */

 //portablity
#ifdef __unix__
    #include <sys/stat.h> //dir recognition
    #include <dirent.h>
    #include <unistd.h>
    #define DIRSEQ "%s/%s"
    #define DIRLETTER '/'
    #define DIRENTRY dir->d_name
#endif // __unix__

#ifdef __WIN32
    #include<windows.h>
    #include<shlwapi.h>
    #define DIRSEQ "%s\\%s"
    #define DIRLETTER '\\'
    #define DIRENTRY dir.cFileName
#endif // __WIN32
#include "list.c"


bool isdir(char* file);
#ifdef __unix__
struct stat sb; //for checking if input is a dir
#endif // __unix__



list* checkdirs( char* path, list* files, char* pattern);
list* checkdirs_r(char* path, char* pattern); //calls checkdirs with extras

//unix and windows have different slashes for folders

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
    #ifdef __unix__
	if (stat(file, &sb) == 0 && S_ISDIR(sb.st_mode)) return true;
	#endif // __unix__
	#ifdef __WIN32
	if(PathIsDirectoryA((LPCTSTR)file)) return true;
	#endif // __WIN32
	else return false;
}

list* checkdirs_r(char* path, char* pattern){ //path is a folder
	int last=strlen(path)-1;
	if(path[last]==DIRLETTER){
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
list* checkdirs( char* path, list* files, char* pattern){
    #ifdef __unix__
	DIR *d;
	struct dirent *dir;
    #endif // __unix__
    #ifdef __WIN32
    HANDLE d;
    WIN32_FIND_DATA dir;
    #endif // __WIN32
	int s_path=strlen(path); //needed for malloc in while
	int s_dname=0, s_both=0;
	#ifdef __WIN32
	char* search_path=path;
	search_path=(char*)calloc(sizeof(char),s_path+2);
	strcpy(search_path,path);
	search_path[s_path]='\\';
	search_path[s_path+1]='*';
	search_path[s_path+2]='\0';
	#endif // __WIN32
	char *path_ptr;						 //malloc ptr
	long int i=0;

	#ifdef __unix__
	d = opendir(path);
	#endif // __unix__
	#ifdef __WIN32
	d = FindFirstFile(search_path,&dir);
	#endif // __WIN32
	list* curr_pos_list=files; //last elem which was added for increased performance
	if (d) {
        #ifdef __unix__
        while ((dir = readdir(d))!= NULL){
        #endif // __unix__
        #ifdef __WIN32
        while(FindNextFile(d, &dir)){
        #endif // __WIN32
            i++;
            //skip dot and dot-dot folders otherwise it would start an infinitive loop
            if ((strcmp(DIRENTRY,"..")==0)||(strcmp(DIRENTRY,".")==0)) {
                continue;
            }
            s_dname=strlen(DIRENTRY);
            s_both=s_path+s_dname+2;
            path_ptr=(char*)calloc(s_both,sizeof(char));
            sprintf(path_ptr,DIRSEQ,path,DIRENTRY);
            if(isdir(path_ptr)){	//check subfolders (relative path from startfolder)
                #ifdef __unix__
                closedir(d);
                #endif //__unix__
                curr_pos_list = checkdirs(path_ptr, curr_pos_list, pattern);
                #ifdef __unix__
				d = opendir(path);
                for(long int j=0;j<i;j++){
					dir=readdir(d);
				}
                #endif // __unix__
                #ifdef __WIN32
				printf("%s\n",search_path);
				d = FindFirstFile(search_path,&dir);
				for (long int j=0;j<i;j++){
                    FindNextFile(d, &dir);
				}
                #endif // __WIN32
			}
            if (s_dname>4){ // file has at least 4 chars
                const char* file_format=&(DIRENTRY[s_dname-4]);
                if (strcmp(pattern,file_format)==0){
                    if(strstr(DIRENTRY,DEF_FILENAME)){
                        curr_pos_list=addlast(new_list(path_ptr,false),curr_pos_list);
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
    #ifdef __unix__
        closedir(d);
    #endif // __unix__
	#ifdef __WIN32
        FindClose(d);
    #endif // __WIN32
	}
	return curr_pos_list;
}
