#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>

#define BUFFER_SIZE 1024

int main(int argc, char *argv[])
{
    if (argc != 4)
    {
        printf("Uso: %s <puerto_propio> <IP_destinatario> <puerto_destinatario>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int puerto_emisor = atoi(argv[1]), puerto_receptor = atoi(argv[3]); // Convertir el puerto (cadena) a entero
    char *server_ip = argv[2];
    int sockfd;
    struct sockaddr_in socket_emisor, socket_receptor;
    char mensaje[] = "Hola hola"; // Mensaje que se enviará al cliente

    // Crear el socket
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0)
    {
        perror("Error al crear el socket");
        exit(EXIT_FAILURE);
    }

    // Configurar la dirección propia
    memset(&socket_emisor, 0, sizeof(socket_emisor));
    socket_emisor.sin_family = AF_INET;
    socket_emisor.sin_addr.s_addr = INADDR_ANY;
    socket_emisor.sin_port = htons(puerto_emisor);

    if (bind(sockfd, (struct sockaddr *)&socket_emisor, sizeof(socket_emisor)) < 0)
    {
        perror("Error al hacer bind");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    // Configurar la dirección del servidor
    memset(&socket_receptor, 0, sizeof(socket_receptor));
    socket_receptor.sin_family = AF_INET;
    socket_receptor.sin_port = htons(puerto_receptor);
    if (inet_pton(AF_INET, server_ip, &socket_receptor.sin_addr) <= 0)
    {
        perror("Dirección IP inválida");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    ssize_t bytes = sendto(sockfd, mensaje, strlen(mensaje), 0, (struct sockaddr *)&socket_receptor, sizeof(socket_receptor));
    if (bytes < 0)
    {
        perror("Error al enviar el mensaje");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    printf("Mensaje enviado: %s\n", mensaje);
    printf("Número de bytes enviados: %zd\n", bytes);

    // Cerrar el socket
    close(sockfd);
    return 0;
}