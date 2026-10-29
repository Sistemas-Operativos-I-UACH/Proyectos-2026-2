#include <stdio.h>
#include <dirent.h>
#include <stdlib.h> // Necesario para atoi()

int main() {
    struct dirent *carpeta;
    DIR *proc = opendir("/proc"); // 1. Abrimos donde Linux guarda los procesos

    printf("PID\tNOMBRE\n");

    // 2. Leemos todo lo que hay dentro de /proc
    while ((carpeta = readdir(proc)) != NULL) {
        
        // EL TRUCO: atoi() convierte texto a número.
        // Si la carpeta se llama "1234", devuelve 1234. 
        // Si se llama "cpuinfo" (no es un proceso), devuelve 0.
        int pid = atoi(carpeta->d_name); 
        
        if (pid > 0) { // Si es mayor a 0, ¡es un proceso real!
            char ruta[50];
            char nombre[100];

            // 3. Armamos la ruta del archivo, ej: "/proc/1234/comm"
            sprintf(ruta, "/proc/%d/comm", pid); 
            
            // 4. Abrimos ese archivo, sacamos la primer palabra y cerramos
            FILE *archivo = fopen(ruta, "r");
            if (archivo != NULL) {
                fscanf(archivo, "%s", nombre); // Lee el nombre automáticamente
                printf("%d\t%s\n", pid, nombre); // Lo imprimimos
                fclose(archivo);
            }
        }
    }
    
    closedir(proc);
    return 0;
}
