#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>

// Función principal del programa
int main(int argc, char *argv[])
{
    int idSocketC;                 // Descriptor del socket del cliente
    struct sockaddr_in ipportserv; // Estructura para almacenar la dirección IP y el puerto del servidor
    char mensaje[1024];            // Buffer para almacenar el mensaje recibido del servidor
    ssize_t bytesRecibidos;        // Variable para almacenar el número de bytes recibidos

    // Verificar que se pasen los argumentos correctos (IP y puerto)
    if (argc != 3)
    {
        printf("Argumentos mal introducidos");
        exit(EXIT_FAILURE);
    }

    // Convertir el argumento del puerto (cadena) a un entero
    int puerto = atoi(argv[2]);

    // Crear el socket del cliente
    idSocketC = socket(AF_INET, SOCK_STREAM, 0);
    if (idSocketC < 0)
    {
        printf("No se pudo crear el socket");
        exit(EXIT_FAILURE);
    }

    // Configurar la estructura de la dirección del servidor
    ipportserv.sin_family = AF_INET;     // Familia de direcciones (IPv4)
    ipportserv.sin_port = htons(puerto); // Puerto del servidor (convertido a formato de red)
    if (inet_pton(AF_INET, argv[1], &ipportserv.sin_addr) <= 0)
    { // Convertir la IP de texto a binario
        printf("Dirección inválida");
        exit(EXIT_FAILURE);
    }

    // Intentar conectarse al servidor
    if (connect(idSocketC, (struct sockaddr *)&ipportserv, sizeof(ipportserv)) < 0)
    {
        printf("Conexión fallida");
        exit(EXIT_FAILURE);
    }

    // Recibir un mensaje del servidor
    bytesRecibidos = recv(idSocketC, mensaje, sizeof(mensaje), 0);

    // Verificar si hubo un error al recibir datos
    if (bytesRecibidos < 0)
    {
        printf("Error al recibir datos");
    }
    else if (bytesRecibidos == 0)
    { // Si el servidor cerró la conexión
        printf("El servidor cerró la conexión\n");
    }
    else
    {
        mensaje[bytesRecibidos] = '\0';                        // Asegurar que el mensaje recibido sea una cadena válida
        printf("Se han recibido %zd bytes\n", bytesRecibidos); // Imprimir el número de bytes recibidos
        printf("Mensaje recibido: %s\n", mensaje);             // Imprimir el mensaje recibido
    }

    // Cerrar el socket
    close(idSocketC);

    return 1; // Finalizar el programa
}