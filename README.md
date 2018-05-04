A mutli-threaded file server that takes in multiple clients and sends a file through AF_INET protocol

gcc main.c -o main -pthread -Wall -g 

./main [port number] [filename]
