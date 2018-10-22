#include "projectheader.h"
#include <stdio.h>

#define SHMEM_SIZE 4098
#define CHAR_BLOC_SIZE 4000
#define SEM_BLOCS "/blocks_to_send"

void* make_shared_mem(size_t size){
  int protection = PROT_READ | PROT_WRITE;
  int visibility = MAP_ANONYMOUS | MAP_SHARED;
  return mmap(NULL, size, protection, visibility, 0,0);
}


char* trimLeadingWhitespace(char* s) {
    while(isspace(*s)) {
        s++;
    }
    return s;
}

int charCompare (const void * a, const void * b) 
{ 
   return strcasecmp (*(const char **) a, *(const char **) b); 
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


  void* shmem1 = make_shared_mem(SHMEM_SIZE);
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
    printf("HELLO AM PARENT\n"); 
    sem_t *S_blocs_to_send = sem_open("S_blocs_to_send", 0);
    sem_t *S_slot = sem_open("S_slot", 0); 
    sem_t *S_bloc = sem_open("S_bloc", 0);


    printf("Parent: opening file and reading lines\n");  
    fp = fopen(text, "r");

    if(fp == NULL){
      printf("Cannot open file\n");
      exit(1);
    }
  
    //TODO this selects input on /n while child runs on periods
    int bloclength = 0;
    int thisLineLength = 0;
    char str[CHAR_BLOC_SIZE]  =  "";
    while((read = getline(&line, &len, fp)) != -1){
      thisLineLength = strlen(line);
      if (bloclength + thisLineLength < CHAR_BLOC_SIZE){
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
        if (thisLineLength < CHAR_BLOC_SIZE){
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
   

 
    bloclength = 0;
   sem_wait(S_blocs_to_send);

   printf("lmfao AFTER WAIT BLOCS TO SEND\n");
   wait(NULL);

  }


  //*************
  //CHILD SECTION
  //*************

  if(pid == 0){
    // sleep(1);  
     printf("Child: examining lines\n");

    char **stringArray;
    int stringArrayLength = 100;
    stringArray = (char **)malloc (sizeof (char *) * stringArrayLength);   
    int currentStringArrayElement = 0;

    //makes it " searchstring " instead of "searchstring"
    char modifiedSearchstring[] = " ";
    strcat(modifiedSearchstring, searchstring);
    strcat(modifiedSearchstring, " ");

    FILE *resultsfp = fopen("shmemResults.txt", "w+");

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
      //printf("blocs to send is zero\n");
      break;
    }

    char thisbloc[CHAR_BLOC_SIZE] = "";
    char *ptr = thisbloc;
    sem_wait(S_bloc);
    memcpy(ptr, shmem1, CHAR_BLOC_SIZE);

    int tokenLength = 0;
    char newline[3] = "\n";
    char period[2] = ".";
    char *token;
    token = strtok(thisbloc, newline);

    //scan the text
    while(token != NULL){ 
      tokenLength = strlen(token);
      //check if searchstring is in line
      if(strstr(token, searchstring) != NULL){
        strcat(token, "\n");
        char* trimedToken = trimLeadingWhitespace(token);

        //add this match to our array of strings
        if (currentStringArrayElement < stringArrayLength){
          stringArray[currentStringArrayElement] = strdup(trimedToken);
        } else {
          stringArray = realloc(stringArray, (2*stringArrayLength) * sizeof(char *));
          stringArrayLength *= 2;
        }
        currentStringArrayElement++;
      }
      token = strtok(NULL, newline);
    }//end of token loop

    sem_post(S_slot);

    }//end of blocs loop
    //printf("end of blocs loop\n");

    qsort(stringArray, currentStringArrayElement, sizeof(char *), charCompare);

    for(int i = 0; i<currentStringArrayElement; i++){
      printf("%s\n\n", stringArray[i]);
      fprintf(resultsfp, "%s\n", stringArray[i]);
    }

    fclose(resultsfp);
    printf("GOT RIGHT BEFORE EXIT CHILD\n");
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

    printf("before file open\n");
    fclose(fp);
    printf("after file close\n");
  }
  printf("before mummap\n");
  munmap(shmem1, SHMEM_SIZE);
  munmap(shmem2, SHMEM_SIZE);
  munmap(shmem3, SHMEM_SIZE);
  munmap(shmem4, SHMEM_SIZE);
  munmap(shmemout1, SHMEM_SIZE);
  munmap(shmemout2, SHMEM_SIZE);
  munmap(shmemout3, SHMEM_SIZE);
  munmap(shmemout4, SHMEM_SIZE);

  printf("after mummap, before sum unlinnk\n");
  sem_unlink("S_blocs_to_send");
  sem_unlink("S_bloc");
  sem_unlink("S_slot");
  printf("after sem unlink\n");

  exit(0);
  //return;

}//end of sharedmemfun        
