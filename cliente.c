#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>

int main(int argc, char *argv[]){
    int idSocketC;
    struct sockaddr_in ipportserv;
    char mensaje[1024];
    ssize_t bytesRecibidos;

    if(argc != 3){
        printf("Argumentos mal introducidos");
        exit(EXIT_FAILURE);
    }

    int puerto = atoi(argv[2]);

    idSocketC = socket(AF_INET, SOCK_STREAM, 0);
    if (idSocketC < 0) {
        printf("No se pudo crear el socket");
        exit(EXIT_FAILURE);
    }

    ipportserv.sin_family = AF_INET;
    ipportserv.sin_port = htons(puerto);
    if (inet_pton(AF_INET, argv[1], &ipportserv.sin_addr) <= 0) {
        printf("Dirección inválida");
        exit(EXIT_FAILURE);
    }

    if(connect(idSocketC, (struct sockaddr *) &ipportserv, sizeof(ipportserv)) < 0){
        printf("Conexión fallida");
        exit(EXIT_FAILURE);
    }

    bytesRecibidos = recv(idSocketC, mensaje, sizeof(mensaje), 0);

    if (bytesRecibidos < 0) {
        printf("Error al recibir datos");
    } else if (bytesRecibidos == 0) {
        printf("El servidor cerró la conexión\n");
    } else {
        mensaje[bytesRecibidos] = '\0'; 
        printf("Mensaje recibido: %s\n", mensaje);
    }

    close(idSocketC);

    return 1;
}