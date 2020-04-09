CC=gcc
CFLAGS=-g
OBJD=obj/
OBJS=$(OBJD)main.o $(OBJD)game.o $(OBJD)print.o $(OBJD)block.o
TARGET=run

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)
	
obj/main.o: main.c
	$(CC) $(CFLAGS) -c -o $@ $<
obj/game.o: game.c
	$(CC) $(CFLAGS) -c -o $@ $<
obj/print.o: print.c
	$(CC) $(CFLAGS) -c -o $@ $<
obj/block.o: block.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f obj/*.o $(TARGET)
