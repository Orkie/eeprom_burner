#include <errno.h>
#include <termios.h>
#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "serial.h"
#include "dump.h"
#include "erase.h"
#include "write.h"

void interface(int portfd);

int main(int argc, char* argv[]) {
  if(argc != 2) {
    printf("Expected use: burn.elf {port}\n");
    return 1;
  }

  char* port = argv[1];
  int fd = open(port, O_RDWR | O_NOCTTY | O_SYNC);
  if(fd < 0) {
    printf("error %d opening %s: %s\n", errno, port, strerror(errno));
    return 1;
  }
  set_interface_attribs(fd, B115200, 0);
  set_blocking(fd, 1);
  tcflush(fd, TCIOFLUSH);
  
  interface(fd);
  
  close(fd);
}

void interface(int portfd) {
  char filename[200];
  char command[20];
  
  while(1) {
    printf("# ");
    scanf("%s", (char*)&command);
    
    if(strcmp("quit", command) == 0) {
      break;
    } else if(strcmp("dump", command) == 0) {
      scanf("%s", (char*)&filename);
      dumpTo(filename, portfd);
    } else if(strcmp("write", command) == 0) {
      scanf("%s", (char*)&filename);
      writeFrom(filename, portfd);
    } else if(strcmp("erase", command) == 0) {
      erase(portfd);
    } else {
      printf("Unknown command! %s\n", command);
    }
  }
}

