CC=cc
CFLAGS=-g -ansi -Wall
OBJS=obj/arraylist.o obj/contact.o obj/file.o obj/main.o
TARGET=contact

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET) $(CFLAGS)

obj/arraylist.o: arraylist.c
	$(CC) -c $< $(CFLAGS) -o $@
obj/contact.o: contact.c
	$(CC) -c $< $(CFLAGS) -o $@
obj/file.o: file.c
	$(CC) -c $< $(CFLAGS) -o $@
obj/main.o: main.c
	$(CC) -c $< $(CFLAGS) -o $@

clean:
	rm -f obj/*.o *txt $(TARGET)
