#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include <errno.h>

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
    unsigned long minflt;
    unsigned long cminflt;
    unsigned long majflt;
    unsigned long cmajflt;
    unsigned long utime;
    unsigned long stime;
    long cutime;
    long cstime;
    long priority;
    long nice;
    long num_threads;
    long itrealvalue;
    unsigned long long starttime;
    unsigned long vsize;
    long rss;
    unsigned long rsslim;
    unsigned long startcode;
    unsigned long endcode;
    unsigned long startstack;
    unsigned long kstkesp;
    unsigned long kstkeip;
    unsigned long signal;
    unsigned long blocked;
    unsigned long sigignore;
    unsigned long sigcatch;
    unsigned long wchan;
    unsigned long nswap;
    unsigned long cnswap;
    int exit_signal;
    int processor;
    unsigned int rt_priority;
    unsigned int policy;
    unsigned long long delayacct_blkio_ticks;
    unsigned long guest_time;
    long cguest_time;
    unsigned long start_data;
    unsigned long end_data;
    unsigned long start_brk;
    unsigned long arg_start;
    unsigned long arg_end;
    unsigned long env_start;
    unsigned long env_end;
    int exit_code;
};



int main (int argc, char *argv[]) {
    struct dirent *entry;
    DIR *proc;
    char proc_dir[] = "/proc";
    FILE *proc_pid;
    struct proc_stat s;
    char path[50];
    char buff[250];
    int is_kernel = 0;

    if ( (proc = opendir(proc_dir)) == NULL) {
        printf("Can't open %s", proc_dir);
        exit(ENOENT);
    }

    while ( (entry = readdir(proc)) != NULL) {
        int pid = atoi(entry->d_name);
        if ( pid == 0) {
            continue;
        }

        sprintf(path, "%s/%s/stat", proc_dir, entry->d_name);
        printf("Abriendo archivo: %s\n", path);
        proc_pid = fopen(path, "r");
        printf("PROC_PID %d\n", proc_pid);
        if (proc_pid != NULL) {
                //"%d (%s[^)]) %c %d %d %d",
            //int read = fread(buf, 
            int parsed = fscanf(proc_pid,
                "%d (%255[^)]) %c %d %d %d %d %d %u",
                &s.pid, s.comm, &s.state, &s.ppid, &s.pgrp, &s.session,
                &s.tty_nr, &s.tpgid, &s.flags
            );


            if (parsed != 6) {
                printf("Error parsing: %s\n", path);
            }

            // Check the PF_KTHREAD bit
            if ( (s.flags & 0x00200000) == 0x00200000 ) {
                 is_kernel = 1;
            } else {
                 is_kernel = 0;
            }
            printf("\n%.30s\n", "------------------------------");
            printf(
                "PID: %d\nComm: (%s)\nState: %c\nPPID: %d\nPGRP: %d\nSession: %d\nProceso de kernel?: %d\n",
                    s.pid, s.comm, s.state, s.ppid, &s.pgrp, s.session, is_kernel
 
             );
            printf("\n%.30s\n", "------------------------------");

            fclose(proc_pid);
        }

    }

}
