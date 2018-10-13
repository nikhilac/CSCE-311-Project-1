#include "projectheader.h"
#include <stdio.h>

#define SHMEM_SIZE 1024

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

  sem_t S_slot;
  sem_t S_bloc;

  sem_init(&S_slot, 1, 1);
  sem_init(&S_bloc, 1, 0);

  void* shmem1 = make_shared_mem(1024);
  void* shmem2 = make_shared_mem(1024);
  void* shmem3 = make_shared_mem(1024);
  void* shmem4 = make_shared_mem(1024);


  void* shmemout1 = make_shared_mem(1024);
  void* shmemout2 = make_shared_mem(1024);
  void* shmemout3 = make_shared_mem(1024);
  void* shmemout4 = make_shared_mem(1024);


  
  pid_t pid = fork();

  //only open the file if we're the parent
  if(pid != 0){ 
 
    printf("Parent: opening file and reading lines\n");  
    fp = fopen(text, "r");

    if(fp == NULL){
      printf("Cannot open file\n");
      exit(1);
    }
  

    int memtarget = 1;
    int bloclength = 0;
    int thisLineLength = 0;
    char str[1000]  =  "";
    while((read = getline(&line, &len, fp)) != -1){
      thisLineLength = strlen(line);
      if (bloclength + thisLineLength < 1000){
        strcat(str, line);
        bloclength += thisLineLength;
      }else{
        //send bloc, empty bloc, cat
        memcpy(shmem1, str, bloclength);
        bloclength = 0;
        strcpy(str, "");
        strcat(str, line);
      }
    }

    //printf("%s", str);
    //printf("%d", bloclength);
//    sem_wait(&S_slot);
    memcpy(shmem1, str, bloclength);
  //  sem_post(&S_bloc);
   
   // printf("Got past post bloc\n");
    //printf(shmem1);
    bloclength = 0;
   wait();

  }

  //print lines if we're the child
  if(pid == 0){
    sleep(1);
    printf("Child: examining lines\n");
    
    char thisbloc[1000] = "";
    char *ptr = thisbloc;
   // sem_wait(&S_bloc);
    memcpy(ptr, shmem1, 1000);
    //sem_post(&S_slot);
    printf("%s", thisbloc);


    char newline[3] = "\n";
    char *token;
    token = strtok(shmem1, newline);
    while(token != NULL){ 
      //printf(" %s\n", token);
      //check if searchstring is in line
      if(strstr(token, searchstring) != NULL){
        //printf(" %s\n", token);
        strcat(token, "\n");
        memcpy(shmemout1, token, strlen(token)); 
      }
      token = strtok(NULL, newline);
    }
    //printf("%s", shmem1);
    exit(0);
  }

  //close file and print lines if we're the parent
  if (pid != 0){
    printf("Parent: read lines and close file\n");
    printf("%s", shmemout1);
    fclose(fp);
  }

  munmap(shmem1, SHMEM_SIZE);
  munmap(shmem2, SHMEM_SIZE);
  munmap(shmem3, SHMEM_SIZE);
  munmap(shmem4, SHMEM_SIZE);
  munmap(shmemout1, SHMEM_SIZE);
  munmap(shmemout2, SHMEM_SIZE);
  munmap(shmemout3, SHMEM_SIZE);
  munmap(shmemout4, SHMEM_SIZE);

  return;

}//end of sharedmemfun        
