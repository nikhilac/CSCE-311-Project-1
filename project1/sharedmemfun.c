#include "projectheader.h"
#include <stdio.h>

#define SHMEM_SIZE 2048
#define CHAR_BLOC_SIZE 2000
#define SEM_BLOCS "/blocks_to_send"

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


  sem_t *S_blocs_to_send = sem_open("S_blocs_to_send", O_CREAT, 0644, 1);
  if(S_blocs_to_send == SEM_FAILED){
     printf("SEMFAILED"); 
  }

  sem_t *S_slot = sem_open("S_slot", O_CREAT, 0644, 1);
  if(S_slot == SEM_FAILED){
     printf("SEMFAILED");
  }


  sem_t *S_bloc = sem_open("S_bloc", O_CREAT, 0644, 0);
  if(S_bloc == SEM_FAILED){
     printf("SEMFAILED");
  }


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
 
    sem_t *S_blocs_to_send = sem_open("S_blocs_to_send", 0);
    sem_t *S_slot = sem_open("S_slot", 0); 
    sem_t *S_bloc = sem_open("S_bloc", 0);


    int fakeint;
    sem_getvalue(S_blocs_to_send, &fakeint);
    printf("From parent, blocs to send:  %d\n", fakeint);

    printf("Parent: opening file and reading lines\n");  
    fp = fopen(text, "r");

    if(fp == NULL){
      printf("Cannot open file\n");
      exit(1);
    }
  

    int bloclength = 0;
    int thisLineLength = 0;
    char str[1000]  =  "";
    while((read = getline(&line, &len, fp)) != -1){
      thisLineLength = strlen(line);
      if (bloclength + thisLineLength < 1000){
        strcat(str, line);
        bloclength += thisLineLength;
      }else{
        //send bloc, empty bloc, cati
        //printf("SENDING BLOC\n");
        //printf(str);
        sem_wait(S_slot);
        //memcpy(shmem1, str, sizeof(str));
        memcpy(shmem1, str, bloclength);
        sem_post(S_bloc);
        bloclength = 0;
        strcpy(str, "");
        if (thisLineLength < 1000){
          strcat(str, line);
        } else {
          printf("LINE TOO LONG\n");
        }
      }
    }
    //printf("%s", str);
    //printf("%d", bloclength);
    sem_wait(S_slot);
    //printf("SENDING BLOC\n");
    //printf(str);
    //memcpy(shmem1, str, sizeof(str));
    memcpy(shmem1, str, bloclength);
    sem_post(S_bloc);
   
   // printf("Got past post bloc\n");
    //printf(shmem1);
    bloclength = 0;
   sem_wait(S_blocs_to_send);
   wait(NULL);

  }

  //child section
  if(pid == 0){
     sleep(1);  
     printf("Child: examining lines\n");
    int avoidinf = 10;

    while(1){
    //avoidinf = avoidinf - 1;
  
    sem_t *sem = sem_open("S_blocs_to_send", 0);
    sem_t *S_slot = sem_open("S_slot", 0);                  
    sem_t *S_bloc = sem_open("S_bloc", 0); 

    int sem_blocs_to_send_val;
    int sem_bloc_val;
    sem_getvalue(sem, &sem_blocs_to_send_val);
    sem_getvalue(S_bloc, &sem_bloc_val);
    //printf("S_blocs_to_send %d\n", sem_blocs_to_send_val);
    if (sem_blocs_to_send_val == 0){
      printf("blocs to send is zero\n");
      break;
    }
    //printf("after zero check\n");

    char thisbloc[1000] = "";
    char *ptr = thisbloc;
    sem_wait(S_bloc);
    memcpy(ptr, shmem1, 1000);
   // printf("%s", thisbloc);

    int tokenLength = 0;
    int returnstrLength = 0;
    char returnstr[1000] = "";
    char newline[3] = "\n";
    char period[2] = ".";
    char *token;
    token = strtok(thisbloc, period);

    //scan the text
    while(token != NULL){ 
      tokenLength = strlen(token);
      //printf(" %s\n", token);
      //check if searchstring is in line
      if(strstr(token, searchstring) != NULL){
        strcat(token, ".\n");
        printf(" %s\n", token);
        if(returnstrLength + tokenLength < 1000){
          strcat(returnstr, token);
          returnstrLength += tokenLength; 
        }else{
          //TODO
        }
      }
      token = strtok(NULL, period);
    }//end of token loop
    sem_post(S_slot);

    }//end of blocs loop
    printf("end of blocs loop\n");

    //printf(returnstr);
    //memcpy(shmemout1, returnstr, strlen(returnstr));
    //printf("%s", shmem1);
    exit(0);
  }

  //close file and print lines if we're the parent
  if (pid != 0){
    printf("Parent: print lines and close file\n");
    //printf("%s", shmemout1);
    //sem_getvalue(&S_blocs_to_send, &blocs_to_send);
    //printf("BLOCS TO SEND %d\n", blocs_to_send);
    if (fp == NULL){
      printf("File pointer is null\n");
    }

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

  sem_unlink("S_blocs_to_send");
  sem_unlink("S_bloc");
  sem_unlink("S_slot");

  return;

}//end of sharedmemfun        
