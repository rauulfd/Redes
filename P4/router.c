#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>

int main(int argc, char *argv[]) {
    if (argc != 3) { // comprobar número de argumentos
        fprintf(stderr, "Uso: %s <archivo> <ip>\n", argv[0]);
        return EXIT_FAILURE;
    }

    FILE *f = fopen(argv[1], "r"); // abrir el archivo con la tabla de reenvío
    if (!f) {
        perror("fopen");
        return EXIT_FAILURE;
    }

    struct in_addr ip_entrada, red_taboa;
    ip_entrada.s_addr = 0; // inicializar a 0

    // Comprobar si la IP de entrada es válida
    if (inet_pton(AF_INET, argv[2], &ip_entrada) != 1) {
        perror("Error en inet_pton");
        fclose(f); // cerrar el archivo antes de salir
        return EXIT_FAILURE;
    }

    uint32_t mascara;

    // Leer y procesar cada línea
    char linea[256], ip_prefijo[40];
    int interfaz, prefijo, prefijo_max = 0, interfaz_seleccionada = 0;

    while (fgets(linea, sizeof(linea), f) != NULL) { // leer línea a línea
        // Comprobar si la línea tiene el formato correcto
        if (sscanf(linea, "%[^,],%d", ip_prefijo, &interfaz) != 2) { // separar IP/prefijo e interfaz
            fprintf(stderr, "Error: línea con formato incorrecto: %s\n", linea);
            fclose(f);
            return EXIT_FAILURE;
        }

        // convertir IP/prefijo a formato binario
        prefijo = inet_net_pton(AF_INET, ip_prefijo, &red_taboa, sizeof(red_taboa));
        if (prefijo < 0) {
            fprintf(stderr, "Error en inet_net_pton (IP/prefijo inválido): %s\n", ip_prefijo);
            fclose(f);
            return EXIT_FAILURE;
        }

        // Calcular la máscara de red según el prefijo
        mascara = (prefijo == 0) ? 0 : htonl(0xFFFFFFFF << (32 - prefijo));

        // Comprobar si la IP de entrada pertenece a esta red y si el prefijo es más largo
        if (((red_taboa.s_addr & mascara) == (ip_entrada.s_addr & mascara)) && (prefijo > prefijo_max)) {
            prefijo_max = prefijo;
            interfaz_seleccionada = interfaz;
        }
    }

    // Mostrar el resultado final
    if (prefijo_max == 0)
        printf("Ninguna red coincide, usando interfaz por defecto (0)\n");
    else
        printf("Interfaz seleccionada: %d con prefijo %d\n", interfaz_seleccionada, prefijo_max);

    fclose(f); // cerrar el archivo
    return EXIT_SUCCESS;
}