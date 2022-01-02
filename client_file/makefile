all: client

gdb=-g

client: client.o sock_init.o utils.o
	gcc -o client client.o sock_init.o utils.o

client.o: client.c
	gcc -c client.c $(gdb)

sock_init.o: sock_init.c
	gcc -c sock_init.c $(gdb)

utils.o: utils.c
	gcc -c utils.c $(gdb)

clean:
	rm -f *.o client
	rm files/*