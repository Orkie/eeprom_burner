#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "dump.h"
#include "crc.h"

#define TOTAL_BYTES (1024*4096)
#define CHUNK_SIZE 256
#define NUMBER_CHUNKS (TOTAL_BYTES/CHUNK_SIZE)

#define BUFFER_SIZE CHUNK_SIZE+2

void dumpTo(char* filename, int fd) {
  printf("Dumping to %s\n", filename);
  
  int dumpFile = open(filename, O_RDWR | O_CREAT);
  if(dumpFile < 0) {
    printf("error %d opening %s: %s\n", errno, filename, strerror(errno));
    return;
  }
  
  // open as something a bit more traditional!
  FILE* ptr = fdopen(fd, "w+");
  
  // send the dump signal
  fputc('D', ptr);
  
  // read each sector out at a time
  int sector;
  uint8_t buffer[BUFFER_SIZE];
  for(sector = 0 ; sector < NUMBER_CHUNKS ; sector++) {
    // transmit the next chunk please!
    fputc('G', ptr);

    // populate the read buffer
    int i;
    for(i = 0 ; i < BUFFER_SIZE ; i++) {
      buffer[i] = fgetc(ptr);
    }
    
    // put it in the output file
    write(dumpFile, buffer, CHUNK_SIZE);
    
    // verify we have received a valid sector
    uint16_t expectedCrc = crc16_ccitt((void*)buffer, CHUNK_SIZE);
    uint16_t actualCrc = *((uint16_t*)&buffer[CHUNK_SIZE]);
    if(expectedCrc != actualCrc) {
      printf("Page %d (0x%.8x) has an invalid CRC, aborting! Expected: %x, Actual: %x\n", sector, CHUNK_SIZE*sector, expectedCrc, actualCrc);
      break;
    }
    
    // chunk done!
    printf("Read page %d: %f%%\n", sector, (100*((double)sector/NUMBER_CHUNKS)));
  }
  
  close(dumpFile);
}
