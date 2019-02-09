main: main.c
	gcc main.c -o main -lpthread

client: client.c
	gcc client.c -o client -lpthread

attacker: attacker.c
	gcc attacker.c -o attacker -lpthread -lm