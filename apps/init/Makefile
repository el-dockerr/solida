CC = gcc
CFLAGS = -Wall -Wextra
TARGET = init

all: $(TARGET)

$(TARGET): init.o
	$(CC) $(CFLAGS) -o $@ $<

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET) *.o