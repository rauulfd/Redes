#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <ctype.h>

#define BUFFER_SIZE 1024

int main(int argc, char *argv[])
{

    if (argc != 5)
    {
        printf("Uso: %s <archivo_entrada> <puerto_propio> <IP_destinatario> <puerto_destinatario>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    char linea[BUFFER_SIZE];
    int puerto_emisor = atoi(argv[2]);
    int puerto_receptor = atoi(argv[4]); // Convertir el puerto (cadena) a entero
    char *server_ip = argv[3];
    int sockfd = 0;
    struct sockaddr_in socket_emisor, socket_receptor;

    // Abrir el archivo de entrada en modo lectura. Si falla, terminar.
    FILE *archivo1 = fopen(argv[1], "r");
    if (archivo1 == NULL)
    {
        printf("Error al abrir el archivo");
        exit(EXIT_FAILURE);
    }

    char nuevoNombre[1024];
    strcpy(nuevoNombre, argv[1]); // Copiar el nombre original
    for (int i = 0; nuevoNombre[i] != '\0'; i++)
    {
        nuevoNombre[i] = toupper((unsigned char)nuevoNombre[i]); // Convertir a mayúsculas
    }

    // Abrir el nuevo archivo para escritura; si falla, cerrar archivo1 y salir.
    FILE *archivo2 = fopen(nuevoNombre, "w");
    if (archivo2 == NULL)
    {
        printf("Error al crear el archivo de salida");
        fclose(archivo1);
        exit(EXIT_FAILURE);
    }

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

    while (fgets(linea, sizeof(linea), archivo1) != NULL)
    {
        // Mostrar por pantalla la línea leída
        printf("Línea leída: %s", linea);

        // Enviar la línea al servidor
        ssize_t bytes = sendto(sockfd, linea, strlen(linea), 0, (struct sockaddr *)&socket_receptor, sizeof(socket_receptor));
        if (bytes < 0)
        {
            perror("Error al enviar el mensaje");
            close(sockfd);
            exit(EXIT_FAILURE);
        }
        printf("Se han enviado %zd bytes\n", bytes);

        // Esperar la respuesta del servidor
        socklen_t addr_len = sizeof(socket_receptor);
        ssize_t bytesRecibidos = recvfrom(sockfd, linea, sizeof(linea) - 1, 0, (struct sockaddr *)&socket_receptor, &addr_len);
        if (bytesRecibidos < 0)
        {
            printf("Error al recibir datos");
            break;
        }
        else if (bytesRecibidos == 0)
        {
            // El servidor cerró la conexión de forma ordenada
            printf("El servidor cerró la conexión\n");
            break;
        }
        else
        {
            linea[bytesRecibidos] = '\0';
            fprintf(archivo2, "%s", linea);
        }

        // sleep(4); //pausar entre envíos
    }

    // Cerrar el socket
    close(sockfd);
    fclose(archivo1);
    fclose(archivo2);
    return 0;
}