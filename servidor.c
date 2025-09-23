#include <sys/types.h>
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
    struct sockaddr_in ipportserv;

    if (idSocketS = socket(AF_INET, SOCK_STREAM, 0) < 0)
    {
        perror("No se pudo crear el socket ");
        exit(EXIT_FAILURE);
    }

    ipportserv.sin_family = AF_INET;
    ipportserv.sin_addr.s_addr = htonl(INADDR_ANY);
    ipportserv.sin_port = htons(8080);

    bind(idSocketS, (struct sockaddr_in))
}