#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>

int main(int argc, char *argv[])
{
    // Validar argumentos
    if (argc != 2)
    {
        printf("Argumentos mal introducidos");
        exit(EXIT_FAILURE);
    }
    int puerto = atoi(argv[1]);

    int idSocketS;
    int sockcon;
    struct sockaddr_in ipportserv;
    struct sockaddr_in ipportcli;
    socklen_t tamano = sizeof(struct sockaddr_in);
    char mensajeRecibido[1024];
    int bytesRecibidos;

    // Preparar estructura de dirección del servidor
    memset(&ipportserv, 0, sizeof(ipportserv));
    ipportserv.sin_family = AF_INET;
    ipportserv.sin_addr.s_addr = htonl(INADDR_ANY); // aceptar peticiones de cualquier interfaz
    ipportserv.sin_port = htons(puerto);            // puerto en orden de red

    // Crear socket TCP
    idSocketS = socket(AF_INET, SOCK_STREAM, 0);

    if (idSocketS < 0)
    {
        perror("No se pudo crear el socket ");
        exit(EXIT_FAILURE);
    }

    if (bind(idSocketS, (struct sockaddr *)&ipportserv, sizeof(struct sockaddr_in)) < 0)
    {
        perror("No se pudo asignar direccion ");
        exit(EXIT_FAILURE);
    }

    // Poner el socket en escucha. Admite 10 solicitudes
    listen(idSocketS, 10);

    // Bucle principal del servidor. aceptar clientes secuencialmente
    while (1)
    {
        // Esperar y aceptar una nueva conexión entrante
        if ((sockcon = accept(idSocketS, (struct sockaddr *)&ipportcli, &tamano)) < 0)
        {
            perror("No se pudo aceptar la conexion");
            continue; // continuar esperando si falla
        }
        printf("Se ha conectado un cliente con ip %s al puerto %d\n", inet_ntoa(ipportcli.sin_addr), ntohs(ipportcli.sin_port));
        // Bucle de recibir datos, ponerlos en mayúsculas y devolverlos
        while (1)
        {
            bytesRecibidos = recv(sockcon, mensajeRecibido, sizeof(mensajeRecibido), 0);
            if (bytesRecibidos <= 0)
            {
                // 0 si el cliente cerró la conexión. <0 si hubo error
                printf("El cliente cerró la conexión o hubo un error\n");
                break;
            }
            // Asegurar terminador nulo para tratarlo como cadena
            mensajeRecibido[bytesRecibidos] = '\0';
            // Convertir a mayúsculas carácter a carácter
            for (int i = 0; mensajeRecibido[i] != '\0'; i++)
            {
                mensajeRecibido[i] = toupper((unsigned char)mensajeRecibido[i]);
            }
            // Enviar de vuelta exactamente los bytes recibidos
            ssize_t bytes = send(sockcon, mensajeRecibido, bytesRecibidos, 0);
            if (bytes < 0)
            {
                perror("Error al enviar mensaje");
                exit(EXIT_FAILURE);
            }
        }
    }

    // Cierre de sockets
    close(sockcon);
    close(idSocketS);

    return 0;
}