#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    // Verificar que se pase el puerto como argumento
    if (argc != 2)
    {
        printf("Argumentos mal introducidos");
        exit(EXIT_FAILURE);
    }
    int puerto = atoi(argv[1]); // Convertir el puerto (cadena) a entero

    int idSocketS; // Descriptor del socket del servidor
    int sockcon;   // Descriptor del socket para la conexión con el cliente
    struct sockaddr_in ipportserv; // Estructura para la dirección del servidor
    struct sockaddr_in ipportcli;  // Estructura para la dirección del cliente
    socklen_t tamano = sizeof(struct sockaddr_in); // Tamaño de la estructura del cliente
    char mensaje[] = "Hola hola"; // Mensaje que se enviará al cliente

    // Inicializar la estructura del servidor con ceros
    memset(&ipportserv, 0, sizeof(ipportserv));
    ipportserv.sin_family = AF_INET; // Familia de direcciones (IPv4)
    ipportserv.sin_addr.s_addr = htonl(INADDR_ANY); // Aceptar conexiones de cualquier interfaz
    ipportserv.sin_port = htons(puerto); // Puerto en formato de red

    // Crear el socket del servidor
    idSocketS = socket(AF_INET, SOCK_STREAM, 0);
    if (idSocketS < 0)
    {
        perror("No se pudo crear el socket ");
        exit(EXIT_FAILURE);
    }

    // Asociar el socket a la dirección y puerto del servidor
    if (bind(idSocketS, (struct sockaddr *)&ipportserv, tamano) < 0)
    {
        perror("No se pudo asignar direccion ");
        exit(EXIT_FAILURE);
    }

    // Poner el socket en modo de escucha para aceptar conexiones
    listen(idSocketS, 10); // Permitir hasta 10 conexiones en cola

    // Aceptar una conexión entrante
    if ((sockcon = accept(idSocketS, (struct sockaddr *)&ipportcli, &tamano)) < 0)
    {
        perror("No se pudo aceptar la conexion");
        exit(EXIT_FAILURE);
    }

    // Mostrar información del cliente conectado
    printf("Se ha conectado un cliente con ip %s al puerto %d\n", 
           inet_ntoa(ipportcli.sin_addr), ntohs(ipportcli.sin_port));

    // Enviar un mensaje al cliente
    ssize_t bytes = send(sockcon, mensaje, sizeof(mensaje), 0);
    printf("Se han enviado %zd bytes\n", bytes);

    // Cerrar el socket de la conexión con el cliente
    close(sockcon);

    // Cerrar el socket del servidor
    close(idSocketS);

    return 0;
}