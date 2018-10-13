#include "projectheader.h"
#include <stdio.h>


void* make_shared_mem(size_t size){
  int protection = PROT_READ | PROT_WRITE;
  int visibility = MAP_ANONYMOUS | MAP_SHARED;
  return mmap(NULL, size, protection, visibility, 0,0);
}

void sharedmemfun(char text[], char searchstring[]){

  FILE *fp;
  char * line = NULL;
  size_t len = 0;
  ssize_t read;

  void* shmem1 = make_shared_mem(128);
  void* shmem2 = make_shared_mem(128);
  void* shmem3 = make_shared_mem(128);
  void* shmem4 = make_shared_mem(128);

  char* testmess = "openthedoor\n";

  
  pid_t pid = fork();

  //only open the file if we're the parent
  if(pid != 0){ 
 
    printf("Parent: opening file and reading lines\n");  
    fp = fopen(text, "r");

    if(fp == NULL){
      printf("Cannot open file\n");
      exit(1);
    }

    while((read = getline(&line, &len, fp)) != -1){
      int realLength = strlen(line);
      memcpy(shmem1, line, realLength);
      //printf(shmem);
      //printf("\n");
    }
    wait();

  }

  //print lines if we're the child
  if(pid == 0){
    printf("Child: printing lines\n");
    printf(shmem1);
    exit(0);
  }

  //close file and print lines if we're the parent
  if (pid != 0){
    printf("Parent: close file\n");
    fclose(fp);
  }

  return;

}//end of sharedmemfun        
