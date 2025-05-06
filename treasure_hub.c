    // PHASE 2
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

// signal handlers 

void sigchld_handler(int sig){ // carefull at the Note: For setting up signal behaviour, you must use sigaction(), not signal().
    int status;
    pid_t pid = wait(&status);
    if (pid == monitor_pid) {
        printf("-> Monitor exited! --> status = %d\n", WEXITSTATUS(status));
        monitor_pid = -1;
        monitor_stopping = 0;
    }
}

void action_list_hunts() {
    DIR* root_directory = opendir(".");
    if (!root_directory) {
        printf("!-> Monitor could not open root dir!\n");
        return;
    }

    struct dirent* entry;
    while ((entry = readdir(root_directory)) != NULL) {
        // printf("Checking dir: %s\n", entry->d_name);    // debug

        if (entry->d_type == DT_DIR && strcmp(entry->d_name, ".") && strcmp(entry->d_name, "..")) {
            char path[MAX];
            snprintf(path, sizeof(path), "%s/treasure.dat", entry->d_name);
            
            struct stat st; // file info
            if (stat(path, &st) == 0) {
                int number_of_treasures = st.st_size / sizeof(Treasure);    // st.st_size = total size of file, in bytes dah
                printf("[Monitor] = Hunt: %s --> total treasures: %d\n", entry->d_name, number_of_treasures);
            }
        }
    }
    closedir(root_directory);
}

void action_list_treasures(const char* hunt) {
    char path[MAX];
    snprintf(path, sizeof(path), "%s/treasure.dat", hunt);

    int fd = open(path, O_RDONLY);
    if (fd == -1) {
        printf("!-> Monitor cannot open treasure file!\n");
        return;
    }

    Treasure current_treasure;
    ssize_t bytes_read;

    printf("[Monitor] = treasures in %s:\n", hunt);
    while ((bytes_read = read(fd, &current_treasure, sizeof(Treasure))) == sizeof(Treasure)) {
        printf(" - ID: %d, User: %s, Coords: %.4f %.4f\n", current_treasure.treasure_ID, current_treasure.user_name, current_treasure.GPS.latitude, current_treasure.GPS.longitude);
        printf(" - clue: %s\n", current_treasure.clue);
        printf(" - value: %d\n", current_treasure.value);
    }

    if (bytes_read == -1) {
        printf("!-> Error reading treasure file");
    }

    close(fd);
}


void action_view_treasure(const char* hunt, int id) {
    char path[MAX];
    snprintf(path, sizeof(path), "%s/treasure.dat", hunt);

    int fd = open(path, O_RDONLY);
    if (fd == -1) {
        printf("!-> Monitor cannot open treasure file!\n");
        return;
    }

    Treasure current_treasure;
    ssize_t bytes_read;
    int found = 0;

    while ((bytes_read = read(fd, &current_treasure, sizeof(Treasure))) == sizeof(Treasure)) {
        if (current_treasure.treasure_ID == id) {
            printf("[Monitor] = found Treasure:\n");
            printf(" - ID: %d, User: %s, Coordinates: %.5f %.5f\n", current_treasure.treasure_ID, current_treasure.user_name, current_treasure.GPS.latitude, current_treasure.GPS.longitude);
            printf(" - Clue: %s\n", current_treasure.clue);
            printf(" - Value: %d\n", current_treasure.value);
            found = 1;
            break;
        }
    }

    if (!found) {
        printf("!-> (view_treasure) treasure with ID %d not found in %s!\n", id, hunt);
    }

    if (bytes_read == -1){
        printf("!-> Error reading treasure file");
    }

    close(fd);
}


void handle_usr1(int sig) {
    FILE *file_output = fopen(".hub_commands_used", "r");
    if (!file_output){
        printf("!-> Monitor cannot read ._hub_commands_used");
        return;
    }

    char buffer[BUFF_SIZE];
    fgets(buffer, sizeof(buffer), file_output);

    buffer[strcspn(buffer, "\n")] = '\0';

    char *tok = strtok(buffer, " ");
    if (strcmp(tok, "list_hunts") == 0) {
        printf("[Monitor] -> list_hunts:\n");
        action_list_hunts();
    }
    else if (strcmp(tok, "stop_monitor") == 0) {
        printf("[Monitor] -> shutting down in 2 seconds...\n");
        usleep(2000000);
        exit(0);
    }
    else if (strcmp(tok, "view_treasure") == 0) {
        char *hunt = strtok(NULL, " ");
        char *id_str = strtok(NULL, " ");
        if (!hunt || !id_str) {
            printf("!-> (view_treasyre) Monitor is missing hunt or treasure ID!\n");
            return;
        }
        printf("[Monitor] -> view_treasure from hunt: %s with ID: %s\n", hunt, id_str);

        action_view_treasure(hunt, atoi(id_str));
    }   
    else if (strcmp(tok, "list_treasures") == 0) {
        char *hunt = strtok(NULL, " ");
        if (!hunt) {
            printf("-> (list_treasures) Monitor missing hunt ID!\n");
            return;
        }
        printf("[Monitor] -> list_treasures from hunt: %s\n", hunt);

        action_list_treasures(hunt);
    } 
    
    else {
        printf("[Monitor] Unknown command: %s\n", tok);
    }

    fclose(file_output);
}

// MONITOR command part

void start_monitor(){
    if(monitor_pid > 0){
        printf("-> Monitor already running!\n");
        return;
    }

    monitor_pid = fork();

    if(monitor_pid < 0){
        perror("FORK error!\n");
        exit(1);
    }
    else if(monitor_pid == 0){  // child procces
        printf("-> Monitor started --> PID = %d\n", getpid());

        // SIGUSR1 handler
        struct sigaction sa_usr1;
        memset(&sa_usr1, 0x00, sizeof(struct sigaction));
        sa_usr1.sa_handler = handle_usr1;
        if (sigaction(SIGUSR1, &sa_usr1, NULL) < 0) {
            perror("Monitor SIGUSR1 sigaction");
            exit(-1);
        }

        while(1){
            pause();
        }

        exit(0);
    }
    else{
        printf("-> monitor process launched (to the moon): PID = %d\n", monitor_pid);
    }
}

void cmd_to_monitor(const char* command){
    if (monitor_pid <= 0) {
        printf("-> NO monitor running\n");
        return;
    }

    FILE *cmd = fopen(".hub_commands_used", "w");
    if (!cmd) {
        printf("ERROR fopen() after stop_monitor\n");
        return;
    }

    fprintf(cmd, "%s\n", command);
    fclose(cmd);

    kill(monitor_pid, SIGUSR1);
}

void stop_monitor(){
    if (monitor_pid <= 0) {
        printf("-> monitor not started yet\n");
        return;
    }

    cmd_to_monitor("stop_monitor");
    monitor_stopping = 1;
}

// the hub itself

int main(){
        // Mainly if i dont handle sigchild, the monitor process would become a zombie after exit

    struct sigaction sa_chld;       // set up a signal handler for SIGCHLD!!  LATER USE sigaction() 
    memset(&sa_chld, 0x00, sizeof(struct sigaction));
    sa_chld.sa_handler = sigchld_handler;
    if (sigaction(SIGCHLD, &sa_chld, NULL) < 0) {
        perror("sigaction SIGCHLD");
        exit(-1);
    }

    char input[BUFF_SIZE];

    while (1) {
        printf("Select command!\n");

        if (!fgets(input, sizeof(input), stdin)) {
            break;
        }

        input[strcspn(input, "\n")] = '\0'; // remove \n

        if(monitor_stopping){
            printf("--> monitor is shutting down ....\n");
            continue;
        }

        if (strcmp(input, "start_monitor") == 0) {
            start_monitor();
        }
        else if (strcmp(input, "stop_monitor") == 0) {
            if (monitor_pid > 0) {
                stop_monitor();
            } 
            else {
                printf("-> HUB: MONITOR not started yet!\n");
            }
        }
        else if (strcmp(input, "exit") == 0) {
            if (monitor_pid > 0) {
                printf("-> HUB: MONITOR ERROR with exit, still run!\n");
            } 
            else {
                printf("-> HUB: Exiting the hub\n");
                break;
            }
        }

        else if (strncmp(input, "list_hunts", 10) == 0 || strncmp(input, "list_treasures", 14) == 0 || strncmp(input, "view_treasure", 13) == 0) {
            cmd_to_monitor(input);  // forward full command to monitor
        }
        else{
            printf("use an existent command pls!\n");
        }
    }

    return 0;
}