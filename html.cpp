#include <string.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <errno.h>
#include <iostream>
#include <fstream>
#include <curl/curl.h>

using namespace std;

struct Node{
    struct Node * next;
    string * address;
};
struct Node * setinel_adr;

size_t write_data(void *buffer, size_t size, size_t nmemb, void *userp){}

int open_address(string url){
  CURL *curl;
  CURLcode res;
 
  curl = curl_easy_init();
  if(curl) {
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);  
    
    /* Perform the request, res will get the return code */ 
    res = curl_easy_perform(curl);
    /* Check for errors */ 
    
    curl_easy_cleanup(curl);
  }
 if(res != 0 && res != 23){
    return 1;
 }else{
    return 0;
 }
}

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
        cout << "Could Not Open" << name.c_str() << endl;
    }
    setinel_adr = node->next;
    return node;     

}
void check_file_for_duds(string filename){
	ifstream inFile;

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
			if(part.find("http") != string::npos){
			//External
				//cerr << "\e[0m \e[0;32m " << "link: " << part << "\e[0m"  << endl;
				if(open_address(part) == 1){
	                                 cerr <<  "\e[0m \e[0;34m " << "External Link Not Found: " + part <<  "\e[0m" << endl;
        	                }

			}else{ 
			//Internal
				string cmd = "cd /" + filename.substr(pos,filename.rfind("/"));
				string wat;
				system(cmd.c_str());
				ifstream ifile(filename.c_str());
				if(!ifile) cerr << "Internal File Not Found" << endl;
			}
		}
	}
	cout << "\e[0;30m";
}

int main(int argc, char* argv[])
{
    cerr << "Scanning Directory for webfiles (.php and .html) ..." << endl;
    DIR *dir;
    struct dirent *ent;
    struct Node * addresses = (struct Node*)malloc(sizeof(struct Node));
    struct Node * tmp;
    //temporary directory plz
    directory_scan("/srv/webspace/ian",dir,ent,addresses);
    cerr << "done." << endl << endl;
    cerr << "The following stage is the most time intensive:" << endl;
    cerr << "searching through files for broken links... " << endl;
    for(tmp = addresses; tmp->next != setinel_adr; tmp = tmp->next){ 
        check_file_for_duds(*(tmp->address));
    }
    cerr << "done. \e[0m" << endl;
    return 0;    
}
