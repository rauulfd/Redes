#include <sys/types.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main()
{

    int idSocketS;
    int idSocketC;
    int sockcon;
    struct sockaddr_in ipportserv;
    struct sockaddr_in ipportcli;
    socklen_t tamano = sizeof(struct sockaddr_in);
    char mensaje[] = "Raul maricon";

    memset(&ipportserv, 0, sizeof(ipportserv));
    ipportserv.sin_family = AF_INET;
    ipportserv.sin_addr.s_addr = htonl(INADDR_ANY); // aceptar peticiones de cualquier interfaz
    ipportserv.sin_port = htons(8080);              // puerto en orden de red

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

    if ((sockcon = accept(idSocketS, (struct sockaddr *)&ipportcli, &tamano)) < 0)
    {
        perror("No se pudo aceptar la conexion");
        exit(EXIT_FAILURE);
    }
    printf("Se ha conectado un cliente con ip %s al puerto %d\n", inet_ntoa(ipportcli.sin_addr), ntohs(ipportcli.sin_port));

    ssize_t bytes = send(sockcon, mensaje, sizeof(mensaje), 0);
    printf("Se han enviado %zd bytes\n", bytes);

    close(sockcon);
    close(idSocketS);

    return 0;
}