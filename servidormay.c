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

    memset(&ipportserv, 0, sizeof(ipportserv));
    ipportserv.sin_family = AF_INET;
    ipportserv.sin_addr.s_addr = htonl(INADDR_ANY); // aceptar peticiones de cualquier interfaz
    ipportserv.sin_port = htons(puerto);            // puerto en orden de red

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

    listen(idSocketS, 10);

    while (1)
    {
        if ((sockcon = accept(idSocketS, (struct sockaddr *)&ipportcli, &tamano)) < 0)
        {
            perror("No se pudo aceptar la conexion");
            continue;
        }
        printf("Se ha conectado un cliente con ip %s al puerto %d\n", inet_ntoa(ipportcli.sin_addr), ntohs(ipportcli.sin_port));
        while(1){
            bytesRecibidos = recv(sockcon, mensajeRecibido, sizeof(mensajeRecibido), 0);
            if (bytesRecibidos <= 0) {
                printf("El cliente cerró la conexión o hubo un error\n");
                break;
            }
            mensajeRecibido[bytesRecibidos] = '\0';
            for (int i = 0; mensajeRecibido[i] != '\0'; i++)
            {
                mensajeRecibido[i] = toupper((unsigned char)mensajeRecibido[i]);
            }
            ssize_t bytes = send(sockcon, mensajeRecibido, bytesRecibidos, 0);
            if (bytes < 0)
            {
                perror("Error al enviar mensaje");
                exit(EXIT_FAILURE);
            }
        }
    }

    close(sockcon);
    close(idSocketS);

    return 0;
}