#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>
#include <resolv.h>

#define MAX_LINEA 100
#define MAX_ENTRADAS 100

typedef struct
{
    struct in_addr red;
    int prefijo;
    int interfaz;
} Entrada;

unsigned long aplicar_mascara(unsigned long ip, int prefijo)
{
    if (prefijo == 0)
        return 0;
    return ip & htonl(~((1 << (32 - prefijo)) - 1));
}

int main(int argc, char *argv[])
{
    if (argc != 3)
    {
        fprintf(stderr, "Uso: %s <archivo_tabla> <ip_entrada>\n", argv[0]);
        return 1;
    }

    FILE *f = fopen(argv[1], "r");
    if (!f)
    {
        perror("Error al abrir el archivo");
        return 1;
    }

    Entrada tabla[MAX_ENTRADAS];
    int n = 0;
    char linea[MAX_LINEA];

    while (fgets(linea, sizeof(linea), f) && n < MAX_ENTRADAS)
    {
        char ip_prefijo[40];
        int interfaz;
        if (sscanf(linea, "%[^,],%d", ip_prefijo, &interfaz) == 2)
        {
            char ip_str[40];
            int prefijo;
            if (sscanf(ip_prefijo, "%[^/]/%d", ip_str, &prefijo) == 2)
            {
                inet_pton(AF_INET, ip_str, &tabla[n].red);
                tabla[n].prefijo = prefijo;
                tabla[n].interfaz = interfaz;
                n++;
            }
        }
    }
    fclose(f);

    for (int i = 0; i < n; ++i)
    {
        char ipbuf[INET_ADDRSTRLEN];
        if (inet_ntop(AF_INET, &tabla[i].red, ipbuf, sizeof ipbuf) == NULL)
        {
            perror("inet_ntop");
            continue;
        }
        printf("%s/%d,%d\n", ipbuf, tabla[i].prefijo, tabla[i].interfaz);
    }
    return 0;
}