#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h> // Necesario para sleep()

struct proc_stat {
    int pid;
    char comm[256];
    char state;
    char state_name[255];
    int ppid;
    int pgrp;
    int session;
    int tty_nr;
    int tpgid;
    unsigned int flags;
};

int main (int argc, char *argv[]) {
    struct dirent *entry;
    DIR *proc;
    char proc_dir[] = "/proc";
    FILE *proc_pid;
    struct proc_stat s;
    char path[64];
    int is_kernel = 0;

    // Bucle infinito para actualizar la lista continuamente
    while (1) {
        // Limpia la pantalla en cada actualización (opcional, pero recomendado)
        system("clear");

        if ((proc = opendir(proc_dir)) == NULL) {
            perror("No se pudo abrir /proc");
            exit(ENOENT);
        }

        // Encabezado alineado de las columnas
        printf("Actualizando cada 2 segundos (Presiona Ctrl+C para salir)\n\n");
        printf("%-8s %-8s %-20s %-6s %-22s %-10s\n", 
               "PID", "PPID", "NOMBRE", "EST.", "SIGNIFICADO ESTADO", "TIPO");
        printf("--------------------------------------------------------------------------------\n");

        while ((entry = readdir(proc)) != NULL) {
            int pid = atoi(entry->d_name);
            if (pid <= 0) {
                continue;
            }

            sprintf(path, "%s/%d/stat", proc_dir, pid);
            proc_pid = fopen(path, "r");
            
            if (proc_pid != NULL) {
                int parsed = fscanf(proc_pid,
                    "%d (%255[^)]) %c %d %d %d %d %d %u",
                    &s.pid, s.comm, &s.state, &s.ppid, &s.pgrp, &s.session,
                    &s.tty_nr, &s.tpgid, &s.flags
                );

                if (parsed != 9) {
                    fclose(proc_pid);
                    continue;
                }

                // Traducir el caracter 'state' a su significado[cite: 2]
                switch (s.state) {
                    case 'R': sprintf(s.state_name, "Ejecutando"); break;
                    case 'S': sprintf(s.state_name, "Esperando (Sleep)"); break;
                    case 'D': sprintf(s.state_name, "Espera Ininterrumpida"); break;
                    case 'Z': sprintf(s.state_name, "Zombi"); break;
                    case 'T': sprintf(s.state_name, "Detenido"); break;
                    case 't': sprintf(s.state_name, "Rastreado"); break;
                    case 'X': case 'x': sprintf(s.state_name, "Muerto"); break;
                    case 'K': sprintf(s.state_name, "Wakekill"); break;
                    case 'W': sprintf(s.state_name, "Paginando"); break;
                    case 'I': sprintf(s.state_name, "Inactivo"); break;
                    default:  sprintf(s.state_name, "Desconocido"); break;
                }

                // Verificar el bit PF_KTHREAD (0x00200000) para saber si es de kernel[cite: 2]
                is_kernel = ((s.flags & 0x00200000) == 0x00200000) ? 1 : 0;

                // Impresion en formato de tabla
                printf("%-8d %-8d %-20.20s %-6c %-22s %-10s\n",
                       s.pid, 
                       s.ppid, 
                       s.comm, 
                       s.state, 
                       s.state_name, 
                       is_kernel ? "Kernel" : "Usuario");

                fclose(proc_pid);
            }
        }

        // Cerrar el directorio en cada iteración para evitar fugas de memoria
        closedir(proc);

        // Pausar 2 segundos antes de volver a leer /proc
        sleep(2);
    }

    return 0;
}
