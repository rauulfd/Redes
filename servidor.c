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

    int idSocketS;                                 // Descriptor del socket del servidor
    int sockcon;                                   // Descriptor del socket para la conexión con el cliente
    struct sockaddr_in ipportserv;                 // Estructura para la dirección del servidor
    struct sockaddr_in ipportcli;                  // Estructura para la dirección del cliente
    socklen_t tamano = sizeof(struct sockaddr_in); // Tamaño de la estructura del cliente
    char mensaje[] = "Hola hola";                  // Mensaje que se enviará al cliente

    // Inicializar la estructura del servidor con ceros
    memset(&ipportserv, 0, sizeof(ipportserv));
    ipportserv.sin_family = AF_INET;                // Familia de direcciones (IPv4)
    ipportserv.sin_addr.s_addr = htonl(INADDR_ANY); // Aceptar conexiones de cualquier interfaz
    ipportserv.sin_port = htons(puerto);            // Puerto en formato de red

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
        close(idSocketS);
        exit(EXIT_FAILURE);
    }

    // Poner el socket en modo de escucha para aceptar conexiones
    listen(idSocketS, 10); // Permitir hasta 10 conexiones en cola

    printf("Servidor escuchando en el puerto %d...\n", puerto);

    while (1) // Bucle infinito para aceptar clientes secuenciales
    {
        // Aceptar una conexión entrante
        if ((sockcon = accept(idSocketS, (struct sockaddr *)&ipportcli, &tamano)) < 0)
        {
            perror("No se pudo aceptar la conexion");
            continue; // Si falla, continuar con la siguiente iteración
        }

        // Mostrar información del cliente conectado
        char ip_cliente[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &(ipportcli.sin_addr), ip_cliente, INET_ADDRSTRLEN);
        printf("Se ha conectado un cliente con ip %s al puerto %d\n",
               ip_cliente, ntohs(ipportcli.sin_port));

        // Enviar un mensaje al cliente
        ssize_t bytes = send(sockcon, mensaje, strlen(mensaje), 0);
        if (bytes < 0)
        {
            perror("Error al enviar el mensaje");
        }
        else
        {
            printf("Se han enviado %zd bytes al cliente\n", bytes);
        }

        // Cerrar el socket de la conexión con el cliente
        close(sockcon);
        printf("Conexión cerrada con el cliente\n");
    }

    // Cerrar el socket del servidor (nunca se alcanza en este caso)
    close(idSocketS);

    return 0;
}