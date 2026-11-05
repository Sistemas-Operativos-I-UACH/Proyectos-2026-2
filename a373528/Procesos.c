#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <ctype.h>
#include <ncurses.h>
#include <unistd.h>


int es_pid(const char *str) {
    for (int i = 0; str[i] != '\0'; i++) {
        if (!isdigit((unsigned char)str[i])) return 0;
    }
    return 1;
}


void obtener_nombre_proceso(const char *pid, char *nombre, size_t tamano) {
    char ruta[256];
    snprintf(ruta, sizeof(ruta), "/proc/%s/stat", pid);

    FILE *archivo = fopen(ruta, "r");
    if (archivo == NULL) {
        strncpy(nombre, "[Terminado]", tamano);
        return;
    }

    int leido_pid;
    char nombre_aux[256];
    if (fscanf(archivo, "%d (%255[^)])", &leido_pid, nombre_aux) == 2) {
        strncpy(nombre, nombre_aux, tamano);
    } else {
        strncpy(nombre, "[Desconocido]", tamano);
    }
    fclose(archivo);
}


void mostrar_procesos() {
    DIR *dir = opendir("/proc");
    if (dir == NULL) {
        printw("Error al abrir /proc\n");
        return;
    }

    struct dirent *entrada;
    int linea = 3;
    int max_y, max_x;
    getmaxyx(stdscr, max_y, max_x);


    mvprintw(0, 0, "MI_TOP - Presiona cualquier tecla para salir");
    mvprintw(2, 0, "%-10s %s", "PID", "NOMBRE DEL PROCESO");
    mvprintw(3, 0, "-----------------------------------");

    while ((entrada = readdir(dir)) != NULL) {
        if (es_pid(entrada->d_name)) {
            char nombre_proceso[256];
            obtener_nombre_proceso(entrada->d_name, nombre_proceso, sizeof(nombre_proceso));
            

            mvprintw(linea, 0, "%-10s %s", entrada->d_name, nombre_proceso);
            linea++;


            if (linea >= max_y - 1) {
                break;
            }
        }
    }
    closedir(dir);
}

int main() {

    initscr();
    cbreak();
    noecho();
    nodelay(stdscr, TRUE); // Hace que getch() NO sea bloqueante (permite que el bucle continúe)
    curs_set(0);

    while (1) {
        clear();
        
        mostrar_procesos();
        
        refresh();


        int ch = getch();
        if (ch != ERR) { 
            break;
        }

        usleep(1000000);
    }


    endwin();
    printf("Programa cerrado correctamente.\n");
    return 0;
}
