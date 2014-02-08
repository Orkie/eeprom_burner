INPUT_FILES=burn.c serial.c dump.c crc.c erase.c write.c
CC=clang
OUTPUT=burn.elf

all: $(INPUT_FILES)
	$(CC) -o $(OUTPUT) $(INPUT_FILES) -I.
