main: main.c
	gcc main.c -o main -lpthread

client: client.c
	gcc client.c -o client -lpthread