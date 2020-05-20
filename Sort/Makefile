CC=cc
CFLAGS=-g -ansi -Wall
OBJS=obj/main.o obj/alloc.o obj/line.o obj/qsort.o
TARGET=sort

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET) $(CFLAGS)

obj/main.o: main.c
	$(CC) -c $< $(CFLAGS) -o $@
obj/alloc.o: alloc.c
	$(CC) -c $< $(CFLAGS) -o $@
obj/line.o: line.c
	$(CC) -c $< $(CFLAGS) -o $@
obj/qsort.o: qsort.c
	$(CC) -c $< $(CFLAGS) -o $@

clean:
	rm -f obj/*.o $(TARGET)
