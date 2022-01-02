all: server

gdb=-g

server: server.o sock_init.o work_thread.o utils.o
	gcc -o server server.o sock_init.o work_thread.o utils.o -lpthread

server.o: server.c
	gcc -c server.c $(gdb)

sock_init.o: sock_init.c
	gcc -c sock_init.c $(gdb)

work_thread.o: work_thread.c
	gcc -c work_thread.c $(gdb)

utils.o: utils.c
	gcc -c utils.c $(gdb)

clean:
	rm -f *.o server
