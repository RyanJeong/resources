CC=cc
CFLAGS=-g -ansi -Wall
OBJS=obj/main.o obj/file.o obj/contact.o
TARGET=cdms

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET) $(CFLAGS)

obj/main.o: main.c
	$(CC) -c $< $(CFLAGS) -o $@

obj/file.o: util.c
	$(CC) -c $< $(CFLAGS) -o $@
obj/contact.o: process.c
	$(CC) -c $< $(CFLAGS) -o $@

clean:
	rm -f obj/*.o $(TARGET)
