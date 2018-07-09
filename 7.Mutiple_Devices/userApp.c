#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#define SUCCESS 0
#define FAILURE -1
#define MAX 50

#define DEVICE_FILE_NAME "/dev/custom0"

char rdbuff[MAX];
char wrbuff[MAX] = "Fortune Favours The Brave";

int main(void)
{
  int fd,retval;
  
  fd = open(DEVICE_FILE_NAME,O_RDWR);
  if(fd < 0)
  {
    perror("Open Failed : ");
    return FAILURE;
  }
  printf("File Open Success\n");
  getchar();
  
  retval = write(fd, wrbuff, strlen(wrbuff));
  if(retval < 0)
  {
    perror("Write Failed : ");
    return FAILURE;
  }
  printf("File Write Success\n");
  getchar();

  retval = read(fd, rdbuff, sizeof(rdbuff));
  if(retval < 0)
  {
    perror("Read Failed : ");
    return FAILURE;
  }
  printf("File Read Success\n");
  printf("%s\n",rdbuff);
  getchar();

  retval = close(fd);
  if(retval < 0)
  {
    perror("Close Failed : ");
    return FAILURE;
  }
  printf("File Close Success\n");
  return SUCCESS;
}
