// Repo: https://github.com/Casperu20/OS_PROJECT

// PHASE 1:

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <unistd.h>     // FOR: write(), read() , unlink() - remove, close() and symlink() for creating the symbolic link
#include <fcntl.h>      // FOR: open() and its flag like O_RDWR, O_CREAT ...
#include <sys/stat.h>   // FOR: mkdir(), stat(), struct stat and the dir/files permisions 0644, 0755 ...

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
    snprintf(link, sizeof(link), "logged-hunt-%s", hunt_dir);
    unlink(link);   // remove if it already exists

    char target_location[BUFF_SIZE];
    snprintf(target_location, sizeof(target_location), "%s/logged_hunt", hunt_dir);
    symlink(target_location, link);  // this fnc creates symbolic links for files or dirs
}

void add(char* hunt_dir, FILE* input){
    // Create the directory for hunt if !exist
    mkdir(hunt_dir, 0755);

    char treasure_file[BUFF_SIZE]; // path to file

    // could have an overflow with just sprintf
    snprintf(treasure_file, sizeof(treasure_file), "%s/treasure.dat", hunt_dir); // WITH .txt FILES WORKS PERFECLTY (that s how I implemented firstly)

    // open the file . IF !exist then create
    int file_tr = open(treasure_file, O_RDWR | O_CREAT | O_APPEND, 0664); // owner rw-, group rw- si others r-
    if(file_tr == -1){
        printf("ERROR WITH OPENNING FILE!\n");
        exit(1);
    }

    // hardcoded initialization for testing
    // Treasure new_treasure = { .treasure_ID = 1, .user_name = "1st_player", .GPS = {123.0087, 10.1223}, .clue = "Near the Oracle Bridge", .value = 100};
    Treasure new_treasure;

    if(fscanf(input, "%d", &new_treasure.treasure_ID) != 1 || fscanf(input, " %[^\n]", new_treasure.user_name) != 1 ||
       fscanf(input, "%f %f", &new_treasure.GPS.latitude, &new_treasure.GPS.longitude) != 2 || fscanf(input, " %[^\n]", new_treasure.clue) != 1 ||
       fscanf(input, "%d", &new_treasure.value) != 1){
            printf("WRONG FORMAT in input file!\n");
            fclose(input);
            close(file_tr);
            exit(1);
    }
    
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

    fclose(input);
}

void list(char* hunt_dir){
    char treasure_file[BUFF_SIZE];
    // for now hope for not having an overflow
    // sprintf(treasure_file, "%s/treasure.dat", hunt_dir);
    snprintf(treasure_file, sizeof(treasure_file), "%s/treasure.dat", hunt_dir);

    int file_tr = open(treasure_file, O_RDWR | O_CREAT | O_APPEND, 0644);
    if(file_tr == -1){
        printf("ERROR WITH OPENNING FILE!\n");
        exit(1);
    }
    
    // First print the hunt name, the (total) file size and last modification time of its treasure file(s). then treasures !

    struct stat temp;   // man: stat = display file or  file system status      | stat(path, DESTINATION) 
    if(stat(treasure_file, &temp) == 0){
        printf(" --- FOUND in hunt -> %s ---\n", hunt_dir);
        printf(" --- Total file size = %ld bytes --- \n", temp.st_size);        // statfs
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
    snprintf(treasure_file, sizeof(treasure_file), "%s/treasure.dat", hunt_dir);

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

void remove_treasure(char* hunt_dir, int ID){
    char treasure_file[BUFF_SIZE];
    snprintf(treasure_file, sizeof(treasure_file), "%s/treasure.dat", hunt_dir);
    char temp_treasure[BUFF_SIZE];
    snprintf(temp_treasure, sizeof(temp_treasure), "%s/temp.dat", hunt_dir);

    int file_tr = open(treasure_file, O_RDWR);
    if(file_tr == -1){
        printf("ERROR oppening file! ( in remove_treasure() )\n");
        exit(1);
    }

    int temp_tr = open(temp_treasure, O_RDWR | O_CREAT | O_TRUNC, 0644);
    if(temp_tr == -1){
        printf("ERROR in remove_treasure() while creating temp_file!\n");
        close(temp_tr);
        exit(1);
    }

    Treasure treasure_read;
    int found = 0;   // flag if we found the treasure

    ssize_t bytes_read;
    while((bytes_read = read(file_tr, &treasure_read, sizeof(Treasure))) == sizeof(Treasure)){
        if(treasure_read.treasure_ID != ID){
            write(temp_tr, &treasure_read, sizeof(Treasure));
        }
        else{
            found = 1;
        }
    }
    close(file_tr);
    close(temp_tr);

    if(found){
        remove(treasure_file);      // man: it can remove a file OR a dir
        rename(temp_treasure, treasure_file);   // replace with the next one
        printf("Treasure[%d] removed!\n", ID);
        create_log(hunt_dir, "REMOVE treasure");
    }
    else{
        remove(temp_treasure);
        printf("Didn't found the treasure[%d]!\n", ID);
        create_log(hunt_dir, "Coudn't remove the treasure!");
    }
}

#include <dirent.h>

void remove_hunt(char* hunt_dir){
    DIR* directory = opendir(hunt_dir);
    if(!directory){
        printf("ERROR oppening directory %s! (in remove_hunt() )\n", hunt_dir);
        exit(1);
    }

    struct dirent* entry;   // points to the directory entry (like a file or subfolder)
    char treasure_file[MAX];  // got a warning from snprintf that it could exceed the limit of 256

    // i ll remove first the files in the directory
    while((entry = readdir(directory))){
        // each entry->d_name is a string(ex: treasure.dat)
        if(strcmp(entry->d_name, ".") && strcmp(entry->d_name, "..")){
            snprintf(treasure_file, sizeof(treasure_file), "%s/%s", hunt_dir, entry->d_name);
            unlink(treasure_file); // remove it
        }
    }
    closedir(directory);

    rmdir(hunt_dir); // the actual removal of directory

    char symbolic_link[BUFF_SIZE];
    snprintf(symbolic_link, sizeof(symbolic_link), "logged_hunt-%s", hunt_dir);
    unlink(symbolic_link);

    printf("Hunt (direcotry) - %s REMOVED!\n", hunt_dir);
}

// MODIFY THE FILE: input_for_add.txt
// for each new treasure u want to add!

int main(int argc, char* argv[]){   
    if(argc < 2){
        printf("USE: ./file OPTION [add / list / view / remove_treasure / remove_hunt] .....\n");
        exit(1);
    }

    char* option = select_option(argv[1]);
    char* hunt_id = argv[2];

    printf("OPTION: %s\n\n", option);

    if(strcmp(option, "add") == 0){
        if(argc < 3){
            printf("USE: ./file add <hunt_dir>\n");
            exit(1);
        }
        FILE* input = fopen("input_for_add.txt", "r");
        if (!input) {
            printf("-> You need a file: input_for_add.txt -> For the details of adding, with the following configuration:\n\n");
            printf("<int>\n<name>\n<float' 'float>\n<clue>\n<int>\n");
            exit(1);
        }
        
        add(hunt_id, input);
    }
    else if(strcmp(option, "list") == 0){
        if(argc < 3){
            printf("USE: ./file list <hunt_dir>\n");
            exit(1);
        }
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
    else if(strcmp(option, "remove_treasure") == 0){
        if(argc < 4){
            printf("USE: ./file remove_treasure <hunt_dir> <id>\n");
            exit(1);
        }
        int ID = atoi(argv[3]);
        remove_treasure(hunt_id, ID);
    }
    else if(strcmp(option, "remove_hunt") == 0){
        if(argc < 3){
            printf("USE: ./file remove_hunt <hunt_dir>\n");
            exit(1);
        }
        remove_hunt(hunt_id);
    }

    return 0;
}