CC=gcc
CFLAGS=-g
OBJS=obj/main.o obj/getblock.o obj/db.o obj/curl.o obj/util.o
TARGET=run

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS) -lcurl
	
obj/main.o: main.c
	$(CC) $(CFLAGS) -c -o $@ $<
obj/getblock.o: getblock.c
	$(CC) $(CFLAGS) -c -o $@ $<
obj/db.o: db.c
	$(CC) $(CFLAGS) -c -o $@ $<
obj/curl.o: curl.c
	$(CC) $(CFLAGS) -c -o $@ $<
obj/util.o: util.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f obj/*.o $(TARGET)
