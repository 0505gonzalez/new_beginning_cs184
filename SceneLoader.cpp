//Funcion declarations
#include <string>
#include "readfile.cpp"
//#include "variables.h"

using namespace std;
void initSceneFile(string);
void clearSceneVars(void);

void initSceneFile(string sceneFile){
    char * fileName;
    fileName = new char[sceneFile.size()+1];
    fileName[sceneFile.size()] = 0;
    memcpy(fileName,sceneFile.c_str(),sceneFile.size());
    
    clearSceneVars();
    
    readfile(fileName);
}

void clearSceneVars(){
  
}