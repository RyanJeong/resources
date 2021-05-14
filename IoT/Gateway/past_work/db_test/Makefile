run: db.o 
	gcc -o run db.o -I/usr/include/mysql -lmysqlclient
db.o: db.c
	gcc -c db.c
all: run
clean:
	rm -f *.o run
