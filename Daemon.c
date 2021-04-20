// Andrés Alam Sánchez Torres A00824854
// José Elías Garza Vázquez - A00824494
// Rodrigo de Jesús Ruiz Kwok A00824488

#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/file.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <syslog.h>
#include <unistd.h>

int EXIT_CODE = 0;

void signalHandler(int SIGNAL) { EXIT_CODE = 1; }

static void daemonize() {
    pid_t pid;

    /* Fork off the parent process */
    pid = fork();

    /* An error occurred */
    if (pid < 0) exit(EXIT_FAILURE);

    /* Success: Let the parent terminate */
    if (pid > 0) exit(EXIT_SUCCESS);

    /* On success: The child process becomes session leader */
    if (setsid() < 0) exit(EXIT_FAILURE);

    /* Catch, ignore and handle signals */
    /*TODO: Implement a working signal handler */
    signal(SIGCHLD, SIG_IGN);
    signal(SIGHUP, SIG_IGN);

    /* Fork off for the second time*/
    pid = fork();

    /* An error occurred */
    if (pid < 0) exit(EXIT_FAILURE);

    /* Success: Let the parent terminate */
    if (pid > 0) exit(EXIT_SUCCESS);

    /* Set new file permissions */
    umask(446);

    /* Change the working directory to the root directory */
    /* or another appropriated directory */
    chdir("/Users/aast/test");

    /* Close all open file descriptors */
    int x;
    for (x = sysconf(_SC_OPEN_MAX); x >= 0; x--) {
        close(x);
    }

    /* Open the log file */
    openlog("firstdaemon", LOG_PID, LOG_DAEMON);
}

int main() {
    printf("Starting daemonize\n");
    daemonize();
    signal(SIGTERM, signalHandler);
    FILE *fp = NULL;

    fp = fopen("Log.txt", "w");

    if (fp != NULL) {
        while (!EXIT_CODE) {
            // TODO: Insert daemon code here.
            syslog(LOG_NOTICE, "First daemon running.");
            fprintf(fp, "Logging to file\n");
            fflush(fp);
            sleep(2);
        }
        fclose(fp);
        remove("Log.txt");
    } else {
        syslog(LOG_NOTICE, "The file is already being used");
        exit(0);
    }
    syslog(LOG_NOTICE, "First daemon terminated.");
    closelog();

    return EXIT_SUCCESS;
}
