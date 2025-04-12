#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

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
        return "remove_treasure";
    }
    else if (strcmp(path, "remove_hunt") == 0){
        return "remove_hunt";
    }
    else{
        printf("Specify a valid option!\n");
        exit(1);
    }
}

// To create a logged_hunt in the directory with treasure
void create_log(char* hunt_dir, char* what_happend){
    char log_file[BUFF_SIZE];
    snprintf(log_file, sizeof(log_file), "%s/logged_hunt", hunt_dir);

    int file_tr = open(log_file, O_RDWR | O_CREAT | O_APPEND, 0644);
    if(file_tr == -1){
        printf("ERROR WITH OPENNING LOG FILE!\n");
        exit(1);
    }

    write(file_tr, what_happend, strlen(what_happend));
    write(file_tr, "\n", 1);

    close(file_tr);
}

void create_symbolic_link(char* hunt_dir){
    char link[BUFF_SIZE];
    snprintf(link, sizeof(link), "logged-hunt-%s.lnk", hunt_dir);
    unlink(link);   // remove if it already exists

    char target_location[BUFF_SIZE];
    snprintf(target_location, sizeof(target_location), "%s/logged_hunt", hunt_dir);
    symlink(target_location, link);  // this fnc creates symbolic links for files or dirs
}

void add(char* hunt_dir){
    // Create the directory for hunt if !exist
    mkdir(hunt_dir, 0755);

    char treasure_file[BUFF_SIZE]; // path to file

    // could have an overflow with just sprintf
    snprintf(treasure_file, sizeof(treasure_file), "%s/treasure.txt", hunt_dir); // found smth about .dat watch later

    // open the file . IF !exist then create
    int file_tr = open(treasure_file, O_RDWR | O_CREAT | O_APPEND, 0664);
    if(file_tr == -1){
        printf("ERROR WITH OPENNING FILE!\n");
        exit(1);
    }

    // The treasure to add needs an initialization
    Treasure new_treasure = { .treasure_ID = 1, .user_name = "1st_player", .GPS = {123.0087, 10.1223}, .clue = "Near the Oracle Bridge", .value = 100};

    // or take them by hand 
    /*
    Treasure t;
    printf("Enter Treasure ID: ");
    scanf("%d", &t.treasure_ID);

    printf("Enter User Name: ");
    scanf(" %[^\n]", t.user_name);

    printf("Enter GPS latitude and longitude: ");
    scanf("%f %f", &t.GPS.latitude, &t.GPS.longitude);

    printf("Enter Clue: ");
    scanf(" %[^\n]", t.clue);

    printf("Enter Value: ");
    scanf("%d", &t.value);
    */

    // now write into the file
    ssize_t binary_written = write(file_tr, &new_treasure, sizeof(Treasure));
    if(binary_written != -1){
        printf("Succeded to add treasure: %ld bytes written\n", binary_written);
    }
    else{
        printf("ERROR WITH WRITING INTO FILE!\n");
    }
    
    close(file_tr);

    create_log(hunt_dir, "ADD treasure");
    create_symbolic_link(hunt_dir);
}

void list(char* hunt_dir){
    char treasure_file[BUFF_SIZE];
    // for now hope for not having an overflow
    sprintf(treasure_file, "%s/treasure.txt", hunt_dir);

    int file_tr = open(treasure_file, O_RDWR | O_CREAT | O_APPEND, 0644);
    if(file_tr == -1){
        printf("ERROR WITH OPENNING FILE!\n");
        exit(1);
    }
    
    // First print the hunt name, the (total) file size and last modification time of its treasure file(s). then treasures !
    struct stat temp;
    if(stat(treasure_file, &temp) == 0){
        printf(" --- FOUND in hunt -> %s ---\n", hunt_dir);
        printf(" --- Total file size = %ld bytes --- \n", temp.st_size);
        printf(" --- Last modification = %s \n", ctime(&temp.st_mtime));
    }

    Treasure found_treasure;
    ssize_t bytes_read;
    while((bytes_read = read(file_tr, &found_treasure, sizeof(Treasure))) > 0){
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

    create_log(hunt_dir, "LIST treasures");
}

void view(char* hunt_dir, int ID){  // to view details of a specified treasure
    char treasure_file[BUFF_SIZE];  // path of file
    snprintf(treasure_file, sizeof(treasure_file), "%s/treasure.txt", hunt_dir);

    int file_tr = open(treasure_file, O_RDWR);
    if(file_tr == -1){
        printf("ERROR OPPENING TREASURE FILE! (in view()!)\n");
        exit(1);
    }

    Treasure treasure_read;
    int found = 0;   // flag if we found the treasure

    ssize_t bytes_read; // it will take the nr
    while((bytes_read = read(file_tr, &treasure_read, sizeof(Treasure))) == sizeof(Treasure)){
        // just check is the same as the ID given and print it
        if(treasure_read.treasure_ID == ID){
            printf(" --- Found the treasure you looked for! ---\n");
            printf("treasure_ID =  %d\n", treasure_read.treasure_ID);
            printf("user = %s\n", treasure_read.user_name);
            printf("coordinates of GPS = %.4f, %.4f\n", treasure_read.GPS.latitude, treasure_read.GPS.longitude);
            printf("clue = %s\n", treasure_read.clue);
            printf("value = %d\n", treasure_read.value);

            found = 1;
            break;
        }
    }  

    if(found == 0){
        printf("Treasure [%d] don't exist!\n", ID);
        create_log(hunt_dir, "view() didn't found treasure searched!");
    }
    else{
        create_log(hunt_dir, "VIEW Treasure");
    }
    close(file_tr);
    
}

// inputul nostru poate fi txt !

int main(int argc, char* argv[]){
    if(argc < 2){
        printf("USE: ./file OPTION [add / list / view / remove_treasure / remove_hunt] .....\n");
        exit(1);
    }

    char* option = select_option(argv[1]);
    char* hunt_id = argv[2];

    printf("OPTION: %s\n\n", option);

    if(strcmp(option, "add") == 0){
        add(hunt_id);
    }
    else if(strcmp(option, "list") == 0){
        list(hunt_id);
    }
    else if(strcmp(option, "view") == 0){
        if(argc < 4){
            printf("USE: ./file view <hunt_dir> <id>\n");
            exit(1);
        }
        int ID = atoi(argv[3]);
        view(hunt_id, ID);
    }




    return 0;
}