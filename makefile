main: main.c
	gcc main.c -o main -lpthread

main1: main1.c
	gcc main1.c -o main1 -lpthread

main2: main2.c
	gcc main2.c -o main2 -lpthread

main3: main3.c
	gcc main3.c -o main3 -lpthread

client: client.c
	gcc client.c -o client -lpthread

attacker: attacker.c
	gcc attacker.c -o attacker -lpthread -lm

all: main.c main1.c main2.c main3.c client.c attacker.c
	gcc main.c -o main -lpthread
	gcc main1.c -o main1 -lpthread
	gcc main2.c -o main2 -lpthread
	gcc main3.c -o main3 -lpthread
	gcc client.c -o client -lpthread
	gcc attacker.c -o attacker -lpthread -lm

clean:
	rm main -f
	rm main1 -f
	rm main2 -f
	rm main3 -f
	rm client -f
	rm attacker -f
	rm server_output_time_aggregated -f