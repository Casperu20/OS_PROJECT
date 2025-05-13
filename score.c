        // phase 3
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


// IN treasure_hub.c also add the handling for calculating score

int main(int argc, char* argv[]){
    if (argc < 2){
        printf("USE: %s ^hunt_directory^ !\n");
        exit(1);
    }

    char path_to_treasure[BUFF_SIZE];
    snprintf(path_to_treasure, sizeof(path_to_treasure), "%s/treasure.dat", argv[1]);

    int fd = open(path_to_treasure, O_RDONLY);
    if (fd == -1) {
        printf("!ERROR opening treasure file!\n");
        exit(1);
    }

    return 0;
}