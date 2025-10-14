#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <ctype.h>

int main(int argc, char *argv[])
{
    int idSocket;
    struct sockaddr_in ippropio;
    struct sockaddr_in ipremoto;
    char mensaje[1024];
    ssize_t bytesRecibidos;

    if (argc != 2)
    {
        printf("Argumentos mal introducidos"); // comprobar numero de args correcto
        exit(EXIT_FAILURE);
    }

    int puerto = atoi(argv[1]);
    // Crear el socket. Si devuelve numero negativo se ha producido un error y aborta
    idSocket = socket(AF_INET, SOCK_DGRAM, 0);
    if (idSocket < 0)
    {
        printf("No se pudo crear el socket");
        exit(EXIT_FAILURE);
    }

    memset(&ippropio, 0, sizeof(ippropio)); // asigna memoria
    ippropio.sin_family = AF_INET;          // para usar IPv4
    ippropio.sin_addr.s_addr = INADDR_ANY;  // recibir de cualquier IP
    ippropio.sin_port = htons(puerto);

    if (bind(idSocket, (struct sockaddr *)&ippropio, sizeof(ippropio)) < 0) // enlaza el socket con IP y puerto
    {
        perror("Error en bind");
        close(idSocket);
        exit(EXIT_FAILURE);
    }

    memset(&ipremoto, 0, sizeof(ipremoto)); // Inicializa la estructura ipremoto
    ipremoto.sin_family = AF_INET;
    ipremoto.sin_addr.s_addr = INADDR_ANY;
    ipremoto.sin_port = 0; // no se rellena el puerto porque no se sabe

    socklen_t addr_len = sizeof(ipremoto);
    bytesRecibidos = recvfrom(idSocket, mensaje, sizeof(mensaje) - 1, 0, (struct sockaddr *)&ipremoto, &addr_len); // recibe de cualquier IP y puerto
    if (bytesRecibidos < 0)
    {
        perror("Error en recvfrom");
        close(idSocket);
        exit(EXIT_FAILURE);
    }
    mensaje[bytesRecibidos] = '\0';
    printf("Mensaje recibido: %s\n", mensaje);

    char ipEmisor[INET_ADDRSTRLEN];                                                                // INET_ADDRSTRLEN es el tamaño suficiente para almacenar una direccion IP
    inet_ntop(AF_INET, &(((struct sockaddr_in *)&ipremoto)->sin_addr), ipEmisor, INET_ADDRSTRLEN); // transforma direccion binaria a cadena. Cast a struct sockaddr para acceder a los cmapos luego
    int puertoEmisor = ntohs(((struct sockaddr_in *)&ipremoto)->sin_port);

    printf("Emisor con IP %s y puerto %d\n", ipEmisor, puertoEmisor);

    close(idSocket);

    return 0;
}