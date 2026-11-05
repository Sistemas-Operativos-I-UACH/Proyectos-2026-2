#include <stdio.h>   
#include <stdlib.h>  
#include <dirent.h>  


int main() {
    DIR *dir = opendir("/proc");   // abre la carpeta /proc (como hacer ls /proc)
    struct dirent *entrada;        // aquí se guarda cada elemento que se lea de /proc
    char ruta[300];                // para armar rutas como /proc/1234/cmdline
    char texto[1024];              // para guardar lo que se lea de los archivos
    FILE *f;                       // representa un archivo abierto

    // Lee un elemento de /proc en cada vuelta, hasta que ya no haya más
    while ((entrada = readdir(dir)) != NULL) {

        // Solo nos interesan las carpetas con nombre numérico (los PIDs).
        // atoi regresa 0 si el nombre no es número (cpuinfo, meminfo, etc.)
        if (atoi(entrada->d_name) == 0)
            continue;   // se salta este elemento y pasa al siguiente

        // Equivale a: echo "Proceso: $d"
        printf("Proceso: /proc/%s\n", entrada->d_name);

        // ---------- cmdline ----------
        // Arma la ruta, por ejemplo: /proc/1234/cmdline
        sprintf(ruta, "/proc/%s/cmdline", entrada->d_name);
        f = fopen(ruta, "r");                       // intenta abrir el archivo
        if (f != NULL) {                            // si no se pudo abrir, se omite (como 2>/dev/null)
            if (fgets(texto, sizeof(texto), f) != NULL)  // lee una línea
                printf("%s", texto);                // la imprime (como cat)
            fclose(f);                              // cierra el archivo
        }
        printf("\n");                               // salto de línea (el echo vacío)

        // ---------- stack ----------
        // Arma la ruta, por ejemplo: /proc/1234/stack
        sprintf(ruta, "/proc/%s/stack", entrada->d_name);
        f = fopen(ruta, "r");                       // solo funciona con sudo
        if (f != NULL) {
            if (fgets(texto, sizeof(texto), f) != NULL)  // fgets lee solo 1 línea = head -1
                printf("%s", texto);
            fclose(f);
        }
    }

    closedir(dir);   // cierra la carpeta /proc
    return 0;        // el programa terminó bien
}
