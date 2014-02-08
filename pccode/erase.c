#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "erase.h"

void erase(int fd) {
  printf("Erasing chip!\n");
  FILE* ptr = fdopen(fd, "w+");
  fputc('E', ptr);
  fflush(ptr);
}

