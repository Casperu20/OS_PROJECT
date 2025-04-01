#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Treasure{
    int treasure_ID;
    char* user_name;
    struct GPS{
        float latitude;
        float longitude;
    } GPS;

    char* clue;
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


int main(int argc, char* argv[]){
    if(argc < 2){
        printf("USE: ./file OPTION file1.txt .....\n");
        exit(1);
    }

    char* option = select_option(argv[1]);

    printf("%s\n", option);


    return 0;
}