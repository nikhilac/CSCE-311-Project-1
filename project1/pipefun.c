#include "projectheader.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

void pipefun(char text[], char searchstring[]){
  FILE *fp;
  int fd[2];
  int fd2[2];
  int fd3[2];

  pipe(fd);
  pipe(fd2);
  pipe(fd3);

  if(pipe(fd) == -1) {
    fprintf(stderr, "Pipe failed.");
    exit(1);
  }
  if(pipe(fd2) == -1) {
    fprintf(stderr, "Pipe failed.");
    exit(1);
  }
  if(pipe(fd3) == -1) {
    fprintf(stderr, "Pipe failed.");
    exit(1);
  }

  pid_t pid = fork();
  if(pid < 0) {
    fprintf(stderr, "Fork failed.");
    return 1;
  }
  //Parent process
  if(pid > 0) {
    close(fd[0]);            //Close reading end of file
    close(fd2[0]);
    close(fd3[1]);          //Close writing end of file

    fp = fopen(text,"r");

    if(fp == NULL) {
      printf("Cannot open file.\n");
      exit(1);
    }
    fseek(fp,0,SEEK_END);
    int sz = ftell(fp);
    rewind(fp);
    char flength[sz];
    fread(flength, sz, 1, fp);

    write(fd[1], flength, (strlen(flength) + 1)); //writes string to child
    write(fd2[1], &sz, sizeof(sz));  //writes size of string to child
    close(fd[1]);    //close writing end of file
    close(fd2[1]);   //close writing end of file
    fclose(fp);
    wait(NULL);      //wait for child to return a string

    char tmp[sz];
    read(fd3[0], tmp, sz);
    printf("%s", tmp);



  }
  //Child process
  if(pid == 0) {
    close(fd[1]);     //Close writing end
    close(fd2[1]);

    int sz;
    read(fd2[0], &sz, sizeof(sz));
    char flength[sz];
    read(fd[0], flength, sz);

    close(fd[0]);   //Close reading end
    close(fd2[0]);
    char *token;
    char tmp[sz];
    token = strtok(flength, ".!?;");
    while(token != NULL) {
      if(strstr(token, searchstring) != NULL) {
        strcat(tmp, token);

      }
      token = strtok(NULL, ".!?;");
    }


    write(fd3[1], tmp, strlen(tmp)+1);
    close(fd3[1]);
  }

  return;
}
      
}
