CC=gcc
CFLAGS=-g
OBJS=db.o
TARGET=run
MYSQL=$(shell mysql_config --cflags --libs) 

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS) -lmysqlclient -lz

db.o: db.c
	$(CC) $(CFLAGS) $(MACRO) -c -o $@ $< $(MYSQL)

clean:
	rm -f db.o $(TARGET)
