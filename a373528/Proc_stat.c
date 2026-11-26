#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>

#define PROC_DIR   "/proc"
#define PF_KTHREAD 0x00200000   /* bit de 'flags' que marca un hilo del kernel */
#define CAMPOS     9            /* cuántos datos extrae el fscanf */

/* Solo los campos que se leen del archivo /proc/[PID]/stat */
struct proc_stat {
    int          pid;
    char         comm[256];
    char         state;
    int          ppid;
    int          pgrp;
    int          session;
    int          tty_nr;
    int          tpgid;
    unsigned int flags;
};

/* Lee /proc/[PID]/stat y llena la estructura. Devuelve 0 si salió bien. */
static int leer_stat(const char *path, struct proc_stat *s)
{
    FILE *f = fopen(path, "r");
    if (f == NULL)
        return -1;               /* el proceso pudo terminar antes de abrirlo */

    int leidos = fscanf(f, "%d (%255[^)]) %c %d %d %d %d %d %u",
                        &s->pid, s->comm, &s->state, &s->ppid, &s->pgrp,
                        &s->session, &s->tty_nr, &s->tpgid, &s->flags);
    fclose(f);

    return (leidos == CAMPOS) ? 0 : -1;
}

/* Traduce la letra del campo estado a su significado */
static const char *significado_estado(char estado)
{
    switch (estado) {
    case 'R': return "Running (en ejecución)";
    case 'S': return "Sleeping (dormido, espera un evento)";
    case 'D': return "Disk sleep (espera E/S, no interrumpible)";
    case 'Z': return "Zombie (terminó, el padre no lo ha recogido)";
    case 'T': return "Stopped (detenido por una señal)";
    case 't': return "Tracing stop (detenido por un depurador)";
    case 'X': return "Dead (muerto)";
    case 'I': return "Idle (inactivo, hilo del kernel)";
    default:  return "Desconocido";
    }
}

/* Imprime los datos de un proceso */
static void imprimir_proceso(const struct proc_stat *s)
{
    char ES[20];
    if (s->flags & PF_KTHREAD) {
        snprintf(ES, sizeof(ES), "%s", "Kernel");
    } else {
        snprintf(ES, sizeof(ES), "%s", "Usuario");
    }
    printf("------------------------------\n");
    printf("\t\tTabla de procesos\n");
    printf("PID: %d\n", s->pid);
    printf("Nombre: %s\n", s->comm);
    printf("Estado: %s\n", significado_estado(s->state));
    printf("PPID: %d\n", s->ppid);
    printf("PGRP: %d\n", s->pgrp);
    printf("Session: %d\n", s->session);
    printf("TTY: %d\n", s->tty_nr);
    printf("TPGID: %d\n", s->tpgid);
    printf("Flag: %s\n", ES);

}    
int main(void)
{
    DIR *proc = opendir(PROC_DIR);
    if (proc == NULL) {
        perror("No se pudo abrir " PROC_DIR);
        return EXIT_FAILURE;
    }

    struct dirent *entry;
    while ((entry = readdir(proc)) != NULL) {
        if (atoi(entry->d_name) == 0)      /* no es número: no es un proceso */
            continue;

        char path[300];
        snprintf(path, sizeof(path), "%s/%s/stat", PROC_DIR, entry->d_name);

        struct proc_stat s;
        if (leer_stat(path, &s) != 0)
            continue;                      /* terminó o no se pudo leer */

        imprimir_proceso(&s);
    }

    closedir(proc);
    return EXIT_SUCCESS;
}