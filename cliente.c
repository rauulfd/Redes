#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(){
    int idSocketC;
    struct sockaddr_in ipportserv;
    socklen_t tam;
    char mensaje[1024];
    ssize_t bytesRecibidos;

    idSocketC = socket(AF_INET, SOCK_STREAM, 0);
    if (idSocketC < 0) {
        perror("No se pudo crear el socket");
        exit(EXIT_FAILURE);
    }

    ipportserv.sin_family = AF_INET;
    ipportserv.sin_port = htons(8080);
    if (inet_pton(AF_INET, "127.0.0.1", &ipportserv.sin_addr) <= 0) {
        perror("Dirección inválida");
        exit(EXIT_FAILURE);
    }

    if(connect(idSocketC, (struct sockaddr *) &ipportserv, sizeof(ipportserv)) < 0){
        perror("Conexión fallida");
        exit(EXIT_FAILURE);
    }

    bytesRecibidos = recv(idSocketC, mensaje, sizeof(mensaje), 0);

    if (bytesRecibidos < 0) {
        perror("Error al recibir datos");
    } else if (bytesRecibidos == 0) {
        printf("El servidor cerró la conexión\n");
    } else {
        mensaje[bytesRecibidos] = '\0'; 
        printf("Mensaje recibido: %s\n", mensaje);
    }

    close(idSocketC);

    return 1;
}