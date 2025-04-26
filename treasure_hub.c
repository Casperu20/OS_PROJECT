#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <unistd.h>     // FOR: write(), read() , unlink() - remove, close() and symlink() for creating the symbolic link
#include <fcntl.h>      // FOR: open() and its flag like O_RDWR, O_CREAT ...
#include <sys/stat.h>   // FOR: mkdir(), stat(), struct stat and the dir/files permisions 0644, 0755 ...

#include <signal.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/wait.h>

#define BUFF_SIZE 256
#define MAX 512

typedef struct Treasure{
    int treasure_ID;
    char user_name[MAX];
    struct GPS{
        float latitude;
        float longitude;
    } GPS;
    char clue[MAX];
    int value;  
} Treasure;

pid_t monitor_pid = -1;
volatile sig_atomic_t monitor_stopping = 0;


// to DO

// MONITOR command part



// signal handlers 

void sigchld_handler(int sig); // carefull at the Note: For setting up signal behaviour, you must use sigaction(), not signal().

// the hub itself


int main(){
        // Mainly if i dont handle sigchild, the monitor process would become a zombie after exit

    struct sigaction sa_chld;       // set up a signal handler for SIGCHLD!!  LATER USE sigaction() 

    sa_chld.sa_handler = sigchld_handler;   // set handler func to a sigchld_handler 
    sigemptyset(&sa_chld.sa_mask);          // block no additional signals
    sa_chld.sa_flags = SA_RESTART;          // if interupted by a signal, restart
    sigaction(SIGCHLD, &sa_chld, NULL);         // REGISTER final for SIGCHILD signal   

    char input[BUFF_SIZE];

    while (1) {
        printf("Select command!\n");

        if (!fgets(input, sizeof(input), stdin)) {
            break;
        }

        input[strcspn(input, "\n")] = '\0'; // remove \n

        if (strcmp(input, "start_monitor") == 0) {
            // start_monitor();
        }

        else if (strcmp(input, "stop_monitor") == 0) {
            if (monitor_pid > 0) {
                // stop_monitor();
            } else {
                printf("-> HUB: MONITOR not started yet!\n");
            }
        }
        else if (strcmp(input, "exit") == 0) {
            if (monitor_pid > 0) {
                printf("-> HUB: MONITOR ERROR with exit, still run!\n");
            } else {
                printf("-> HUB: Exiting the hub\n");
                break;
            }
        }
    }
}