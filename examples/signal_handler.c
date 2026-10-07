#include <stdio.h>    // For printf
#include <stdlib.h>   // For exit
#include <signal.h>   // For sigaction, sigemptyset, SIGTERM
#include <unistd.h>   // For getpid, pause, write
#include <string.h>   // For strlen

/**
 * @brief A global flag to signal the main loop to exit.
 * * 'volatile sig_atomic_t' is the only type of variable
 * that can be safely modified inside a signal handler
 * and read outside of it.
 */
volatile sig_atomic_t please_exit = 0;

/**
 * @brief The custom signal handler for SIGTERM.
 *
 * This function will be executed when the process receives SIGTERM.
 *
 * @param signum The signal number that was caught (will be SIGTERM).
 */
void handle_sigterm(int signum) {
    // WARNING: Using printf/puts or other complex functions in a signal
    // handler is not technically safe, as they are not "async-signal-safe".
    // The *only* truly safe way to write from a handler is using write().
    
    const char *msg = "\nCaught SIGTERM! Cleaning up and exiting gracefully.\n";
    
    // write() is async-signal-safe and can be used here.
    // STDOUT_FILENO is file descriptor 1 (standard output).
    // We use sizeof() - 1 to avoid writing the null terminator.
    write(STDOUT_FILENO, msg, sizeof(msg) - 1);

    // Set the global flag to signal the main loop
    please_exit = 1;
}

int main() {
    // This 'struct sigaction' will define the new behavior for SIGTERM
    struct sigaction sa;

    // 1. Set the handler function
    sa.sa_handler = handle_sigterm;

    // 2. Clear the signal mask (sa_mask)
    // This ensures no other signals are blocked *during* the
    // execution of our handler.
    sigemptyset(&sa.sa_mask);

    // 3. Set flags. 
    // SA_RESTART is often used to auto-restart interrupted syscalls (like read/write),
    // but for this simple exit, we can just use 0.
    sa.sa_flags = 0;

    // 4. Register the signal handler for SIGTERM
    // This tells the OS: "When you get SIGTERM, call sa.sa_handler instead
    // of doing the default (terminating)."
    if (sigaction(SIGTERM, &sa, NULL) == -1) {
        perror("Error: cannot handle SIGTERM");
        exit(EXIT_FAILURE);
    }

    // Print our Process ID (PID) so the user knows who to 'kill'
    printf("Process running with PID: %d\n", getpid());
    printf("Send 'kill -TERM %d' or 'kill -15 %d' to this process from another terminal.\n", getpid(), getpid());
    printf("Waiting for signal...\n");

    // 5. Wait for the signal
    // The loop will run until the handler sets 'please_exit' to 1.
    // pause() efficiently waits for *any* signal to arrive.
    while (please_exit == 0) {
        pause(); // Suspend execution until a signal is caught
    }

    // This code is executed after the handler has run
    printf("Main loop is exiting. Goodbye!\n");
    
    return EXIT_SUCCESS;
}
