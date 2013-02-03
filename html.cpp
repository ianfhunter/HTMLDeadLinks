#include <string.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <errno.h>
#include <vector>
#include <iostream>
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
                    cerr << *(node->address) <<endl;
                    node->next = (struct Node*)malloc(sizeof(struct Node));
                    node = node->next; 
                }
            }
            
        }
    cout << "\e[0;m";       //reset colours
    }
    closedir (dir); 
    } else {
    /* could not open directory */
        cout << "Could Not Open" << name.c_str() << endl;
	}
	setinel_adr = node->next;
    return node;     

}

int main(int argc, char* argv[])
{
    DIR *dir;
    struct dirent *ent;
    struct Node * addresses = (struct Node*)malloc(sizeof(struct Node));
    struct Node * tmp;
    directory_scan("/srv/webspace/ian",dir,ent,addresses);
    cerr << "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~" << endl;
    for(tmp = addresses; tmp != setinel_adr; tmp = tmp->next){
        cerr << *(tmp->address) << endl;
    }
    return 0;    
}


