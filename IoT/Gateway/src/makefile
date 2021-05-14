CC=gcc
CFLAGS=-g
OBJS=obj/main.o obj/util.o obj/process.o obj/crc.o obj/server.o obj/rf.o
TARGET=run

ifeq ($(mode),alpha)
MACRO=-DALPHA -DPRINT
else ifeq ($(mode),beta)
MACRO=-DBETA -DPRINT
else ifeq ($(mode),release)
MACRO=-DRELEASE -DPRINT
else ifeq ($(mode),alpha_log)
MACRO=-DALPHA -DLOG
else ifeq ($(mode),beta_log)
MACRO=-DBETA -DLOG
else ifeq ($(mode),release_log)
MACRO=-DRELEASE -DLOG
endif

ifndef MACRO
MACRO=-DBETA -DPRINT
endif
all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS) -lpthread

obj/main.o: main.c
	$(CC) $(CFLAGS) $(MACRO) -c -o $@ $<
obj/util.o: util.c
	$(CC) $(CFLAGS) $(MACRO) -c -o $@ $<
obj/process.o: process.c
	$(CC) $(CFLAGS) $(MACRO) -c -o $@ $<
obj/crc.o: crc.c
	$(CC) $(CFLAGS) $(MACRO) -c -o $@ $<
obj/server.o: server.c
	$(CC) $(CFLAGS) $(MACRO) -c -o $@ $<
obj/rf.o: rf.c
	$(CC) $(CFLAGS) $(MACRO) -c -o $@ $<

clean:
	rm -f obj/*.o $(TARGET)
