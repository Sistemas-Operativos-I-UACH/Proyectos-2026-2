#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>

const char* obtener_estado(char c) {
    switch (c) {
        case 'R': return "Running";
        case 'S': return "Sleeping";
        case 'D': return "Disk sleep";
        case 'Z': return "Zombie";
        case 'T': return "Stopped";
        case 't': return "Tracing stop";
        case 'W': return "Paging/Waking";
        case 'X': case 'x': return "Dead";
        case 'K': return "Wakekill";
        case 'P': return "Parked";
        case 'I': return "Idle";
        default:  return "Desconocido";
    }
}

int main() {
    struct dirent *entry;
    DIR *proc = opendir("/proc");
    
    if (!proc) {
        printf("Error: No se puede abrir /proc\n");
        return 1;
    }

    printf("%-8s %-25s %-7s %-14s %-8s %-8s %-8s %-8s\n",
        "PID", "COMM", "STATE", "STATUS", "PPID", "PGRP", "SESSION", "KERNEL");
    printf("-------- ------------------------- ------- -------------- -------- -------- -------- --------\n");

    while ((entry = readdir(proc)) != NULL) {
        // Ignorar directorios que no sean números (PID)
        if (atoi(entry->d_name) == 0) continue;

        char path[256];
        snprintf(path, sizeof(path), "/proc/%s/stat", entry->d_name);
        
        FILE *f = fopen(path, "r");
        if (f) {
            int pid, ppid, pgrp, session, tty_nr, tpgid;
            char comm[256], state;
            unsigned int flags;

            // Leer exactamente los primeros 9 campos hasta las banderas (flags)
            if (fscanf(f, "%d (%255[^)]) %c %d %d %d %d %d %u",
                       &pid, comm, &state, &ppid, &pgrp, &session, &tty_nr, &tpgid, &flags) == 9) {
                
                // Si el bit PF_KTHREAD (0x00200000) está activo, es de Kernel
                int is_kernel = (flags & 0x00200000) ? 1 : 0;
               
                printf("%-8d %-25.25s %-7c %-14s %-8d %-8d %-8d %-8s\n",
                       pid, comm, state, obtener_estado(state),
                       ppid, pgrp, session, is_kernel ? "Si" : "No");
            }
            fclose(f);
        }
    }
    
    closedir(proc);
    return 0;
} 

