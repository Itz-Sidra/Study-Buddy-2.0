CC = gcc
CFLAGS = -Wall -O2 -mconsole
TARGET = server

all: $(TARGET)

$(TARGET): server.c
	$(CC) $(CFLAGS) -o $(TARGET) server.c -lws2_32

clean:
	rm -f $(TARGET).exe
