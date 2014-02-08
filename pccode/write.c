#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "write.h"
#include "crc.h"

#define TOTAL_BYTES (1024*4096)
#define PAGE_SIZE 256
#define NUMBER_PAGES (TOTAL_BYTES/PAGE_SIZE)

#define BUFFER_SIZE PAGE_SIZE

void writeFrom(char* filename, int fd) {
  printf("Writing from %s\n", filename);
  
  int dumpFile = open(filename, O_RDWR);
  if(dumpFile < 0) {
    printf("error %d opening %s: %s\n", errno, filename, strerror(errno));
    return;
  }
  
  // open as something a bit more traditional!
  FILE* ptr = fdopen(fd, "w+");
  
  // send the dump signal
  fputc('P', ptr);
  
  int page;
  uint8_t buffer[BUFFER_SIZE];
  for(page = 0 ; page < NUMBER_PAGES ; page++) {
    read(dumpFile, buffer, PAGE_SIZE);
    
    int i;
    for(i = 0 ; i < PAGE_SIZE ; i++) {
      fputc(buffer[i], ptr);
    }
    
    uint16_t incomingCrc = ((((uint16_t)fgetc(ptr)) << 8)&0xFF00) | (((uint16_t)fgetc(ptr))&0xFF);
    uint16_t expectedCrc = crc16_ccitt((void*)buffer, PAGE_SIZE);
    
    if(expectedCrc != incomingCrc) {
      printf("Page %d (0x%.8x) has an invalid CRC once it had reached the programmer, aborting! Expected: %x, Actual: %x\n", page, PAGE_SIZE*page, expectedCrc, incomingCrc);
      break;
    }
    
    // chunk done!
    printf("Wrote page %d: %f%%\n", page, (100*((double)page/NUMBER_PAGES)));
  }
  
  close(dumpFile);
}
