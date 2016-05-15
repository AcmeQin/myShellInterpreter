#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <regex.h>
#include <unistd.h>
#include <dirent.h>

#define MAX 1024
#define PIPE_NAME_LEN 100



regex_t regex;
int reti;
char dir[MAX];
DIR *d;
struct dirent *dirCont;
char *Args[4];

int isExternal(char *cmd);
void innerExecute(char *cmd);

int main(int argc, char *argv[]) {
  char* pipeName;
  char buf[MAX];
  int bytesread;
  size_t len = 0;
  ssize_t read;
  Args[0]=NULL;
  Args[1]=NULL;
  Args[2]=NULL;
  Args[3]=NULL;

  // Get pipe name, connect pipe
  printf("Please enter pipe name. \n");
  // for test
  // scanf("%s",pipeName);
  pipeName = "testpipe";
  printf("pipeName: %s\n", pipeName);
  FILE *fd = fopen( pipeName, "r" );

  while (1) {
    while (fgets(buf,MAX,fd)!=NULL) {
      buf[strcspn(buf,"\n")]=0;
      if (getcwd(dir, sizeof(dir)) != NULL)
        ;
      else
        perror("getcwd() error");
      printf("myshell@linux:%s$ %s\n", dir, buf);

      if (isExternal(buf)){
        pid_t child_pid;
        int status;
        child_pid = fork();
        if(child_pid!=0){
          waitpid(-1,&status,0);
        }else{

          char * rawArgs;
          Args[0]="/bin/ls";
          rawArgs=strtok(cmd," ");

          rawArgs=strtok(NULL," ");
          char temp1[PIPE_NAME_LEN];
          strcpy(temp1,rawArgs);
          Args[1]=temp1;

          rawArgs=strtok(NULL," ");
          char temp2[PIPE_NAME_LEN];
          strcpy(temp2,rawArgs);
          Args[2]=temp2;

          printf("%s\n", Args[1]);
          printf("%s\n", Args[2]);

          execv(Args[0],Args);
          exit(0);
        }

      }else{
        innerExecute(buf);
      }

    }
    if (strcmp(buf,"exit")==0)
      break;
  }

  fclose(fd);
  regfree(&regex);
  return 0;
}


// This method determine the type of command and
// initialize arguments for outer commands
int isExternal(char *cmd){
  // Compile regular expression
  reti = regcomp(&regex,"^ls[ ].*$",0);
  if (reti) {
    fprintf(stderr, "Could not compile regex\n");
    exit(1);
  }
  reti = regexec(&regex, cmd, 0, NULL, 0);
  if (!reti) {




    // Args[1]="rawArgs";
    // rawArgs=strtok(NULL," ");
    // printf("%s\n", rawArgs);
    // strcpy(Args[2],rawArgs);
    return 1;
  }
  else if (reti == REG_NOMATCH) {
  }
  else {
    regerror(reti, &regex, cmd, sizeof(cmd));
    fprintf(stderr, "Regex match failed: %s\n", cmd);
    exit(1);
  }

  return 0;
}

// This method execute the inner commands
void innerExecute(char *cmd){

  // pwd command
  if (strcmp(cmd,"pwd")==0){
    if (getcwd(dir, sizeof(dir)) != NULL)
      fprintf(stdout, "%s\n", dir);
    else
      perror("getcwd() error");
    return;
  }

  // cd command
  /* Execute regular expression */
  reti = regcomp(&regex,"^cd[ ].*$",0);
  if (reti) {
    fprintf(stderr, "Could not compile regex\n");
    exit(1);
  }
  reti = regexec(&regex, cmd, 0, NULL, 0);
  if (!reti) {
      size_t len = strlen(cmd);
      memmove(cmd, cmd+3, len - 3 + 1);
      reti = chdir(cmd);
  }
  else if (reti == REG_NOMATCH) {
  }
  else {
      regerror(reti, &regex, cmd, sizeof(cmd));
      fprintf(stderr, "Regex match failed: %s\n", cmd);
      exit(1);
  }
}
