#include <string.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <errno.h>
#include <iostream>
#include <fstream>
using namespace std;

struct Node{
    struct Node * next;
    string * address;
};
struct Node * setinel_adr;

struct Node * directory_scan(string name, DIR* dir,struct dirent *ent, struct Node *node){
    if ((dir = opendir (name.c_str())) != NULL) {
    string place;
    string valid_path;
    char *token;
    char *token_prev;
    /* print all the files and directories within directory */
    while ((ent = readdir (dir)) != NULL) {
        if( strcmp(ent->d_name,".") != 0 && strcmp(ent->d_name,"..") != 0  ){
            if(ent->d_type ==''){
                place = name.append("/");
                place = place.append(string(ent->d_name));
                node = directory_scan(place,dir,ent,node);
                }else{                
                token = strtok(ent->d_name, ".");
                while (token != NULL) {
                    token_prev = token;
                    token = strtok(NULL, ".");
                }
                if(strcmp(token_prev,"html") == 0 || strcmp(token_prev,"php") == 0){
                    //I could do the check here, but out of visual preference, i'm going to do it in a different stage
                    node->address = new string (name + "/" + ent->d_name + "." + token_prev );
                    node->next = (struct Node*)malloc(sizeof(struct Node));
                    node = node->next; 
                }
            }
            
        }
    }
    closedir (dir); 
    } else {
    /* could not open directory */
        cout << "Could Not Open" << name.c_str() << endl;
	}
	setinel_adr = node->next;
    return node;     

}
void check_file_for_duds(string filename){
	ifstream inFile;
	cerr  << "\e[0;31m" << filename<< "\e[0;3m" << endl;
	inFile.open(&filename[0], ios::in);
	if(!inFile){
		cerr << "error reading file" << endl;
	}
	string part;
	while (!inFile.eof()) {
 		inFile >> part;
		int pos = part.find("href=");
		if(pos != std::string::npos){
			part = part.substr(pos+6,part.rfind("\"") -6);
			//part is our current html files
			cerr << part << endl;
		}
	}
}

int main(int argc, char* argv[])
{
    DIR *dir;
    struct dirent *ent;
    struct Node * addresses = (struct Node*)malloc(sizeof(struct Node));
    struct Node * tmp;
    directory_scan("/srv/webspace/ian",dir,ent,addresses);
    for(tmp = addresses; tmp->next != setinel_adr; tmp = tmp->next){ 
        check_file_for_duds(*(tmp->address));
    }
    return 0;    
}


