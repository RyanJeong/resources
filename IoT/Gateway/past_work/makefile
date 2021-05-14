all: run

obj/main.o: main.c
	gcc -c -o $@ $<
obj/rf.o: rf.c
	gcc -c -o $@ $<
obj/thread.o: thread.c
	gcc -c -o $@ $<
obj/crc.o: crc.c
	gcc -c -o $@ $<

OBJS=obj/main.o obj/rf.o obj/thread.o obj/crc.o

run: $(OBJS)
	gcc -o run $(OBJS)  -lpthread

clean:
	rm -f obj/*.o run
