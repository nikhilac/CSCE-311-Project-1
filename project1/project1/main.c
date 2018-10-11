#include "projectheader.h"



int main(int argc, char *argv[]){


  if (argc != 3) {
    printf("Invalid number of arguments, type this: ./runme [ipc method] [input file]\n");
    printf("Ipc method can be either pipe, socket, or sharedmem\n");
    exit(0);
  }
  

  
  char filename[255] = "";
  if (strlen(argv[2]) >= sizeof(filename)){
    printf("filename too long\n");
    exit(1);
  }
  strncpy(filename, argv[2], strlen(argv[2]));
  //filename[strlen(argv[3])] = '\0';
  //might have to worry about null terminator when assigning strings?
  
  if (strcmp(argv[1] , "pipe") == 0){
    pipefun(filename);
  } else if (strcmp(argv[1], "socket") == 0){
    socketfun(filename);
  } else if (strcmp(argv[1], "sharedmem") == 0){
    sharedmemfun(filename);
  } else {
    printf("Invalid ipc type. Valid ipc types are: pipe, socket, or sharedmem");
  }
  return(0);
 
}
