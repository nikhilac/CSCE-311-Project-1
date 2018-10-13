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

  void* shmem = make_shared_mem(128);
  char* testmess = "openthedoor\n";

  /*
  //make shared mem
  int shmid;
  key_t shmkey = 12345;
  shmid = shmget(shmkey,1024, 0666 | IPC_CREAT);
  char *shmpointer = shmat(shmid, NULL, 0);
  */
  fp = fopen(text, "r");
  if(fp == NULL){
      printf("Cannot open file\n");
      exit(1);
  }
  
  //char bigstr[] = "";
  while((read = getline(&line, &len, fp)) != -1){
    //printf("real line:\n%s", line);
    //strcat(line, "\n");
    //printf(line);
    //printf("%d", strlen(line));
    int realLength = strlen(line);
    memcpy(shmem, line, realLength);
    printf(shmem);
    printf("\n");
  }
  
  //strncpy(shmem, bigstr, sizeof(bigstr));
  //printf("%s", shmem);
  //strncpy(shmpointer, testmess, sizeof(testmess));

  //printf("%s", shmpointer);

  pid_t pid = fork();

  //only open the file if we're the parent
  if(pid != 0){ 
 
    //printf("Parent: opening file and reading lines\n");  
    fp = fopen(text, "r");

    if(fp == NULL){
      printf("Cannot open file\n");
      exit(1);
    }

    while((read = getline(&line, &len, fp)) != -1){
     // strncpy(shmpointer, line, sizeof(line));
    }
   // printf("%s", shmpointer);
    wait(); 
  }

  //print lines if we're the child
  if(pid == 0){
    //printf("Child: printing lines\n");
    //printf("%s", shmem);
    exit(0);
  }

  //close file and print lines if we're the parent
  if (pid != 0){
    //printf("Parent: close file\n");
    fclose(fp);
  }
//  shmdt(&shmpointer);
  //shmctl(shmid,IPC_RMID, NULL);

  return;
}        
