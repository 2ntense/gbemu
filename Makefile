CC = gcc
CFLAGS = -Wall -DDEBUG
LDFLAGS =
TARGET = gbemu
BIN_PATH = ./bin
SRC_PATH = ./src

all: $(TARGET)

$(TARGET): *.o
	mkdir -p $(BIN_PATH);\
	$(CC) $(CFLAGS) -o $(BIN_PATH)/$(TARGET) $(wildcard $(SRC_PATH)/*.o) $(LDFLAGS)

*.o:
	cd $(SRC_PATH);\
	$(CC) $(CFLAGS) -c *.c	

debug:
	cd $(SRC_PATH); \
	$(CC) -g $(CFLAGS) -c *.c; \
	cd ..; \
	mkdir -p $(BIN_PATH); \
	$(CC) -g $(CFLAGS) -o $(BIN_PATH)/$(TARGET) $(wildcard $(SRC_PATH)/*.o) $(LDFLAGS)

run:
	$(BIN_PATH)/$(TARGET)

clean:
	rm -rf roms/ bin/ src/*.o