#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <fcntl.h>
// #include <sys/stat.h>

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


char* select_option(const char* path){
    if(strcmp(path, "add") == 0){
        return "add";
    }
    else if (strcmp(path, "list") == 0){
        return "list";
    }
    else if (strcmp(path, "view") == 0){
        return "view";
    }
    else if (strcmp(path, "remove_treasure") == 0){
        return "remote_treasure";
    }
    else if (strcmp(path, "remove_hunt") == 0){
        return "remove_hunt";
    }
    else{
        printf("Specify a valid option!\n");
        exit(1);
    }
}

void add(char* hunt_dir){
    char treasure[BUFF_SIZE];
    // for now hope for not having an overflow with sprintf so try with sn
    snprintf(treasure, sizeof(treasure), "%s/treasure.txt", hunt_dir); // found smth about .dat watch later

    // open the file . IF !exist then create
    int file_tr = open(treasure, O_RDWR);
    if(file_tr == -1){
        printf("ERROR WITH OPENNING FILE!\n");
        exit(1);
    }

    // The treasure to add needs an initialization
    Treasure new_treasure = { .treasure_ID = 1, .user_name = "1st_player", .GPS = {123.0087, 10.1223}, .clue = "Near the Oracle Bridge", .value = 100};

    // now write into the file
    ssize_t binary_written = write(file_tr, &new_treasure, sizeof(Treasure));
    if(binary_written != -1){
        printf("Succeded to add treasure: %ld bytes written\n", binary_written);
    }
    else{
        printf("ERROR WITH WRITING INTO FILE!\n");
    }
    
    close(file_tr);
}

void list(char* hunt_dir){
    char treasure[BUFF_SIZE];
    // for now hope for not having an overflow
    sprintf(treasure, "%s/treasure.txt", hunt_dir);

    int file_tr = open(treasure, O_RDWR | O_CREAT | O_APPEND, 0644);
    if(file_tr == -1){
        printf("ERROR WITH OPENNING FILE!\n");
        exit(1);
    }
    

    Treasure found_treasure;
    ssize_t bytes_read;
    while((bytes_read = read(file_tr, &found_treasure, sizeof(Treasure))) > 0){
        printf("FOUND:  \n");
        printf("treasure_ID =  %d\n", found_treasure.treasure_ID);
        printf("user = %s\n", found_treasure.user_name);
        printf("coordinates of GPS = %.4f, %.4f\n", found_treasure.GPS.latitude, found_treasure.GPS.longitude);
        printf("clue = %s\n", found_treasure.clue);
        printf("value = %d\n", found_treasure.value);
        printf("\n");
    }

    if(bytes_read == -1){
        printf("ERROR READING FILE!\n");
    }

    close(file_tr);
}


// inputul nostru poate fi txt !

int main(int argc, char* argv[]){
    if(argc < 2){
        printf("USE: ./file OPTION file1.txt .....\n");
        exit(1);
    }

    char* option = select_option(argv[1]);
    char* hunt_id = argv[2];

    printf("%s\n", option);

    if(strcmp(option, "add") == 0){
        add(hunt_id);
    }
    else if(strcmp(option, "list") == 0){
        list(hunt_id);
    }




    return 0;
}