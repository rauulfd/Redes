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
    char linea[1024];
    ssize_t bytesRecibidos;

    if(argc != 4){
        printf("Argumentos mal introducidos");
        exit(EXIT_FAILURE);
    }

    FILE *archivo1 = fopen(argv[1], "r");
    if (archivo1 == NULL) {
        printf("Error al abrir el archivo");
        exit(EXIT_FAILURE);
    }

    char nuevoNombre[1024];
    strcpy(nuevoNombre, argv[1]); // Copiar el nombre original
    for (int i = 0; nuevoNombre[i] != '\0'; i++) {
        nuevoNombre[i] = toupper((unsigned char)nuevoNombre[i]); // Convertir a mayúsculas
    }

    // Abrir el nuevo archivo para escritura
    FILE *archivo2 = fopen(nuevoNombre, "w");
    if (archivo2 == NULL) {
        printf("Error al crear el archivo de salida");
        fclose(archivo1);
        exit(EXIT_FAILURE);
    }

    int puerto = atoi(argv[3]);

    idSocketC = socket(AF_INET, SOCK_STREAM, 0);
    if (idSocketC < 0) {
        printf("No se pudo crear el socket");
        exit(EXIT_FAILURE);
    }

    ipportserv.sin_family = AF_INET;
    ipportserv.sin_port = htons(puerto);
    if (inet_pton(AF_INET, argv[2], &ipportserv.sin_addr) <= 0) {
        printf("Dirección inválida");
        exit(EXIT_FAILURE);
    }

    if(connect(idSocketC, (struct sockaddr *) &ipportserv, sizeof(ipportserv)) < 0){
        printf("Conexión fallida");
        exit(EXIT_FAILURE);
    }

    while (fgets(linea, sizeof(linea), archivo1) != NULL) {
        printf("Línea leída: %s", linea);
        ssize_t bytes = send(idSocketC, linea, strlen(linea), 0);
        if (bytes < 0) {
            printf("Error al enviar datos");
            break;
        }
        printf("Se han enviado %zd bytes\n", bytes);

        bytesRecibidos = recv(idSocketC, linea, sizeof(linea) - 1, 0);
        if (bytesRecibidos < 0) {
            printf("Error al recibir datos");
            break;
        } else if (bytesRecibidos == 0) {
            printf("El servidor cerró la conexión\n");
            break;
        } else {
            linea[bytesRecibidos] = '\0';
            fprintf(archivo2, "%s", linea);
        }
    }


    fclose(archivo1);
    fclose(archivo2);
    close(idSocketC);

    return 0;
}