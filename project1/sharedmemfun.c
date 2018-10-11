#include "projectheader.h"
#include <stdio.h>


void sharedmemfun(char text[]){

  FILE *fp;
  char *line = NULL;
  size_t len = 0;
  ssize_t read;


  fp = fopen(text, "r");

  if(fp == NULL){
    printf("Cannot open file");
    exit(1);
  }


  while((read = getline(&line, &len, fp)) != -1){
     printf("%s", line);
  }

  fclose(fp);
  return;
}        
