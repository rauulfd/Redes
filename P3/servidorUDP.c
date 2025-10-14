#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <ctype.h>

// Función para convertir una cadena a mayúsculas
void convertir_a_mayusculas(char *mensaje)
{
    for (int i = 0; mensaje[i] != '\0'; i++)
    {
        mensaje[i] = toupper((unsigned char)mensaje[i]);
    }
}

int main(int argc, char *argv[])
{
    int idSocket;
    struct sockaddr_in ippropio;
    struct sockaddr_in ipremoto;
    char linea[1024];
    ssize_t bytesRecibidos;

    if (argc != 2)
    {
        printf("Argumentos mal introducidos\n");
        exit(EXIT_FAILURE);
    }

    int puerto = atoi(argv[1]);

    // Crear el socket
    idSocket = socket(AF_INET, SOCK_DGRAM, 0);
    if (idSocket < 0)
    {
        perror("No se pudo crear el socket");
        exit(EXIT_FAILURE);
    }

    memset(&ippropio, 0, sizeof(ippropio));
    ippropio.sin_family = AF_INET;
    ippropio.sin_addr.s_addr = INADDR_ANY;
    ippropio.sin_port = htons(puerto);

    if (bind(idSocket, (struct sockaddr *)&ippropio, sizeof(ippropio)) < 0)
    {
        perror("Error en bind");
        close(idSocket);
        exit(EXIT_FAILURE);
    }

    socklen_t addr_len = sizeof(ipremoto);

    while (1)
    {
        // Recibir mensaje del cliente
        bytesRecibidos = recvfrom(idSocket, linea, sizeof(linea) - 1, 0, (struct sockaddr *)&ipremoto, &addr_len);
        if (bytesRecibidos < 0)
        {
            perror("Error en recvfrom");
            close(idSocket);
            exit(EXIT_FAILURE);
        }

        linea[bytesRecibidos] = '\0';
        printf("Mensaje recibido: %s\n", linea);

        // Convertir el mensaje a mayúsculas
        convertir_a_mayusculas(linea);

        // Enviar el mensaje convertido de vuelta al cliente
        if (sendto(idSocket, linea, strlen(linea), 0, (struct sockaddr *)&ipremoto, addr_len) < 0)
        {
            perror("Error en sendto");
            close(idSocket);
            exit(EXIT_FAILURE);
        }

        char ipEmisor[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &ipremoto.sin_addr, ipEmisor, INET_ADDRSTRLEN);
        int puertoEmisor = ntohs(ipremoto.sin_port);

        printf("Mensaje enviado a emisor con IP %s y puerto %d\n", ipEmisor, puertoEmisor);
    }

    close(idSocket);

    return 0;
}