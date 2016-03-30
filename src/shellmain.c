#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>

#define MAX 1024
#define PIPE_NAME_LEN 100

int main(int argc, char *argv[]) {
  char pipeName[PIPE_NAME_LEN];
  char *buf=NULL;
  int bytesread;
  size_t len = 0;
  ssize_t read;
  printf("Please enter pipe name. \n");
  scanf("%s",pipeName);
  // fgets(pipeName,PIPE_NAME_LEN,stdin);
  printf("pipeName: %s\n", pipeName);

  FILE *fd = fopen( pipeName, "r" );

  while ((read = getline(&buf, &len, fd)) != -1) {
    printf("%s\n", buf);
  }

  fclose(fd);

  return 0;
}
