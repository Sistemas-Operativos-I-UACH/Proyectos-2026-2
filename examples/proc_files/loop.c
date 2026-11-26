#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>

void handle_sigterm(int);

int keep_running = 1;

int main(int argc, char *argv[]) {
    int my_pid = getpid();
    printf("Program running. Process ID (PID): %d\n", my_pid);
    printf("To terminate gracefully, run: kill %d\n", my_pid);

    // Set up the sigaction structure
    struct sigaction action;
    action.sa_handler = handle_sigterm; // Point to our handler function
    sigemptyset(&action.sa_mask);       // Block no other signals during execution
    action.sa_flags = 0;                // No special options

    // Bind the SIGTERM signal to our custom action
    if (sigaction(SIGTERM, &action, NULL) < 0) {
        perror("Error registering SIGTERM handler");
        return EXIT_FAILURE;
    }

    for(int i = 0; keep_running; i++) {
        printf("En loop cochino PID (%d) %d\n", my_pid, i);
    }
}


// The custom signal handler function
void handle_sigterm(int signum) {
    // Only use async-signal-safe functions inside a handler (like write instead of printf)
    // Here we simply set the flag to break the main loop
    printf("Me llegó SIGTERM, terminando...\n");
    keep_running = 0;
}

