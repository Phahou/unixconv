/*
 * this program displays the names of all files in the current directory.
 */

#include "list.c"


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


bool isdir(char* file);
#ifdef __unix__
struct stat sb; //for checking if input is a dir
#endif // __unix__



list* checkdirs( char* path, list* files,const char* pattern);
list* checkdirs_r(char* path,const char* pattern); //calls checkdirs with extras

//unix and windows have different slashes for folders

/*
 * dir: directory to search
 * sort: sort list with bubblesort?
 * pattern: file ending to search for
 * what it doesnt do: open the files it returns for sorting the false postives out
*/


list* dirs(char* dir, bool sort,const char* pattern) {
	list* path=checkdirs_r(dir, pattern);
	if (sort) bubbleSort(path);
	return path;
}
/*int main(int argc,char** argv){
	list* a=dirs(argv[2],true,argv[1]);
	printall(a);
	del_complete_list(a);
	return 0;
}*/

/* dir functions */
bool isdir(char* file){
	#ifdef __unix__
	if (stat(file, &sb) == 0 && S_ISDIR(sb.st_mode)) return true;
	#endif // __unix__
	#ifdef __WIN32
	if( PathIsDirectoryA((LPCSTR)file) ) return true;
	#endif // __WIN32
	else return false;
}

list* checkdirs_r(char* path,const char* pattern){ //path is a folder
	size_t last=strlen(path)-1;
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
list* checkdirs( char* path, list* files,const char* pattern){
	#ifdef __unix__
		DIR *d;
		struct dirent *dir;
	#endif // __unix__
	#ifdef __WIN32
		HANDLE d;
		WIN32_FIND_DATAA dir;
	#endif // __WIN32

	size_t s_path=strlen(path); //needed for malloc in while
	size_t s_dname=0, s_both=0, s_both_prev=0;

	#ifdef __WIN32
		char* search_path=path;
		search_path=(char*)calloc(sizeof(char),s_path+2);
		strcpy(search_path,path);
		search_path[s_path]='\\';
		search_path[s_path+1]='*';
		search_path[s_path+2]='\0';
	#endif // __WIN32

	char *path_ptr=(char*)calloc(16,sizeof(char));     //malloc ptr
	long int i=0;

	#ifdef __unix__
		d = opendir(path);
	#endif // __unix__
	#ifdef __WIN32
		d = FindFirstFileA(search_path,&dir);
	#endif // __WIN32

	list* curr_pos_list=files; //last elem which was added for increased performance
	
	if (d) {
	#ifdef __unix__
		while ((dir = readdir(d))!= NULL){
	#endif // __unix__
	#ifdef __WIN32
		while(FindNextFileA(d, &dir)){
	#endif // __WIN32
			i++;
		//skip dot and dot-dot folders otherwise it would start an infinitive loop
			if ((strcmp(DIRENTRY,"..")==0)||(strcmp(DIRENTRY,".")==0)) {  continue; }
			
		//alloc memory while saving unnecessary malloc calls
		//saves ~ 1 / 3 of malloc calls lol (path_ptr 16 byte init saves maybe 1/20 or so )
			s_dname=strlen(DIRENTRY);
			s_both=s_path+s_dname+2;
			if(s_both>s_both_prev){
				path_ptr=(char*)realloc(path_ptr,s_both*2*sizeof(char));
				s_both_prev=s_both*2;
				memset(path_ptr,0,s_both_prev); //set zero
			} else {
				memset(path_ptr,0,s_both+1);
			}

		//create filepath string
			sprintf(path_ptr,DIRSEQ,path,DIRENTRY);
			
		//check subfolders (relative path from startfolder)
			if(isdir(path_ptr)){
				#ifdef __unix__
					closedir(d);
				#endif //__unix__
			//add new files / update file-list
				curr_pos_list = checkdirs(path_ptr, curr_pos_list, pattern);

			//reset dir-entry pointer
				#ifdef __unix__
					d = opendir(path);
					for(long int j=0;j<i;j++){  dir=readdir(d); }
				#endif // __unix__

				#ifdef __WIN32
					printf("%s\n",search_path);
					d = FindFirstFileA(search_path,&dir);
					for (long int j=0;j<i;j++){ FindNextFileA(d, &dir); }
				#endif // __WIN32
			}

		//search for pattern
			if (s_dname>4){ // file has at least 4 chars
				const char* file_format=&(DIRENTRY[s_dname-4]);
				switch(strcmp(pattern,file_format)){
					case 0:
						if(strstr(DIRENTRY,DEF_FILENAME)){
							curr_pos_list=addlast(new_list(path_ptr,true), curr_pos_list);
							break;
						}/* else fall through */

					default:/*  do nothing because it will be filled with zeros
								anyhow after each iteration */;
				}
			}
		}
	//End of while loop ~> cleaning up d
	#ifdef __unix__
		closedir(d);
	#endif // __unix__
	#ifdef __WIN32
		FindClose(d);
		free(search_path);
	#endif // __WIN32
	}
	free(path_ptr);
	
	return curr_pos_list;
}
