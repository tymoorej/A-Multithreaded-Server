#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>
#include "common.h"
#include "timer.h"

int array_size, server_port, server_file_descriptor;
double times_array[COM_NUM_REQUEST];
int time_index = 0;
char *server_IP;
char **theArray;
pthread_t thread_ids[COM_NUM_REQUEST];
pthread_mutex_t lock_whole_array;
pthread_mutex_t time_lock;

void setup_socket(){
    struct sockaddr_in sock_var;
    server_file_descriptor = socket(AF_INET,SOCK_STREAM,0);
    sock_var.sin_addr.s_addr = inet_addr(server_IP);
    sock_var.sin_port = server_port;
    sock_var.sin_family = AF_INET;

    // force bind
    int options = 1;
    if (setsockopt(server_file_descriptor, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &options, sizeof(options)) < 0){
        perror("Socket Bind Settings Failed\n");
        exit(0);
    }


    if (bind(server_file_descriptor, (struct sockaddr*) &sock_var, sizeof(sock_var)) < 0){
        perror("Socket Bind Failed\n");
        exit(0);
    }

    printf("Socket binded correctly\n");

    if (listen(server_file_descriptor, COM_NUM_REQUEST) < 0){
        perror("Socket Bind Failed\n");
        exit(0);
    }

    printf("Server now listning\n");
}


void* handle_client(void *fd){
    int client_file_descriptor = (int) (long) fd;
    double start, end;
    char buffer[COM_BUFF_SIZE];
    
    read(client_file_descriptor, buffer, COM_BUFF_SIZE);
    //start
    GET_TIME(start);

    printf("Recieved From Client: %s\n", buffer);
    
    ClientRequest request;
    memset((char *) &request, 0, sizeof(request));
    
    char *msg = strdup(buffer);
    ParseMsg(msg, &request);

    if (!request.is_read){
        pthread_mutex_lock(&lock_whole_array);
        setContent(request.msg, request.pos, theArray);
        pthread_mutex_unlock(&lock_whole_array);
    }

    char output_buffer[COM_BUFF_SIZE];
    getContent(output_buffer, request.pos, theArray);

    //end
    GET_TIME(end);
    write(client_file_descriptor, output_buffer, COM_BUFF_SIZE);
    close(client_file_descriptor);

    pthread_mutex_lock(&time_lock);
    times_array[time_index] = end - start;
    time_index++;
    time_index %= COM_NUM_REQUEST;
    pthread_mutex_unlock(&time_lock);
    pthread_exit(0);
}


void run_server(){
    int i, client_file_descriptor;
    while(1){
       for (i = 0; i < COM_NUM_REQUEST; i++){
            client_file_descriptor = accept(server_file_descriptor, NULL, NULL);
            if (client_file_descriptor < 0){
                perror("Accept failed\n");
                exit(0);
            }
            printf("Connected to client %d\n", client_file_descriptor);
            pthread_create(&thread_ids[i], NULL, handle_client, (void*) (long) client_file_descriptor);
        }
        saveTimes(times_array, COM_NUM_REQUEST);
    }
}

int main(int argc, char* argv[]){
    if (argc != 4){
        printf("Invalid number of arguments.\nArguments should be: Size of array, Server IP, Server port.\n");
        exit(0);
    }
    array_size = atoi(argv[1]);
    server_IP = argv[2];
    server_port = atoi(argv[3]);

    pthread_mutex_init(&lock_whole_array, NULL);
    pthread_mutex_init(&time_lock, NULL);

    int i;
    theArray = malloc(array_size * sizeof(char *));
    for (i = 0; i < array_size; i++){
        theArray[i] = malloc(COM_BUFF_SIZE * sizeof(char));
    }

    for (i = 0; i < array_size; i++){
        sprintf(theArray[i], "String %d: the initial value", i);
    }

    setup_socket();

    run_server();

}


