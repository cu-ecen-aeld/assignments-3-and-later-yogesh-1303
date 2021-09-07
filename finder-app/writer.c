#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <syslog.h>

int main(int argc, char **argv){

    openlog(NULL, 0, LOG_USER);
    if(argc !=3 ){
    printf("\n2 arguments required\n1: Path of the file\n2: string to be written\n");
    exit(1);
  }

  int fd = open(argv[1], O_CREAT | O_WRONLY | O_TRUNC, 0644);
  if(fd == -1)
  {
	  syslog(LOG_ERR,"\nproblem in openening the file");
	  exit(1);
  }
  ssize_t num;
  syslog(LOG_DEBUG, "\nwriting %s to file %s", argv[2], argv[1]);
  num = write(fd, argv[2], strlen(argv[2]));
  if(num == -1)
  {
  	syslog(LOG_ERR, "\nerror writing the file");
  	exit(1);
  }
  closelog();
  return 0;
}
