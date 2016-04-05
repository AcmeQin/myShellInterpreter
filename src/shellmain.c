#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
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

void execute(char *cmd);

int main(int argc, char *argv[]) {
  char pipeName[PIPE_NAME_LEN];
  char buf[MAX];
  int bytesread;
  size_t len = 0;
  ssize_t read;
  // Compile regular expression
  reti = regcomp(&regex,"^cd[ ].*$",0);
  if (reti) {
    fprintf(stderr, "Could not compile regex\n");
    exit(1);
  }

  printf("Please enter pipe name. \n");
  scanf("%s",pipeName);
  // fgets(pipeName,PIPE_NAME_LEN,stdin);
  printf("pipeName: %s\n", pipeName);


  FILE *fd = fopen( pipeName, "r" );
  while (1) {
    while (fgets(buf,MAX,fd)!=NULL) {
      buf[strcspn(buf,"\n")]=0;
      printf(">>> %s\n", buf);
      execute(buf);
    }
    if (strcmp(buf,"exit")==0)
      break;
  }

  fclose(fd);
  regfree(&regex);
  return 0;
}

// This method gets command input, uses regular expression to category and executes correspondingly

void execute(char *cmd){
  // pwd command
  if (strcmp(cmd,"pwd")==0){
    if (getcwd(dir, sizeof(dir)) != NULL)
      fprintf(stdout, "%s\n", dir);
    else
      perror("getcwd() error");
    return;
  }

  // ls command
  if (strcmp(cmd,"ls")==0){
    if (getcwd(dir, sizeof(dir)) != NULL){
      d=opendir(dir);
      if (d){
        while ((dirCont = readdir(d)) != NULL){
          printf("%s\n", dirCont->d_name);
        }
        closedir(d);
      }
    }
    else
      perror("getcwd() error");
    return;
  }

  // cd command
    /* Execute regular expression */
  reti = regexec(&regex, cmd, 0, NULL, 0);
  if (!reti) {
      puts("Match");
      size_t len = strlen(cmd);
      memmove(cmd, cmd+3, len - 3 + 1);
      reti = chdir(cmd);
  }
  else if (reti == REG_NOMATCH) {
      puts("No match");
  }
  else {
      regerror(reti, &regex, cmd, sizeof(cmd));
      fprintf(stderr, "Regex match failed: %s\n", cmd);
      exit(1);
  }
}
