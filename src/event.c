#include "includes/event.h"
#include "includes/ansii_print.h"
#include "includes/fight.h"
#include "includes/map.h"
#include "includes/utils.h"
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#define MAX_PATH_LENGTH 256

typedef struct {
    char storyPath[MAX_PATH_LENGTH];
    char id[10];
    char context[MAX_PATH_LENGTH];
    char situation[MAX_PATH_LENGTH];
    int event;
} StoryChoice;

char* initializeStoryChoice(int villageID, int placeID, int scenarioID)
{
    char storyFolderPath[128];

    if (getcwd(storyFolderPath, sizeof(storyFolderPath)) != NULL) {
        // Append the "story" folder to the current working directory
        strncat(storyFolderPath, "/story", sizeof(storyFolderPath) - strlen(storyFolderPath) - 1);

        char scenarioPath[MAX_PATH_LENGTH];
        snprintf(scenarioPath, sizeof(scenarioPath), "%s/village_%d/places/place_%d/scenarios/scenario_%d", storyFolderPath, villageID, placeID, scenarioID);

        // Allocate memory for the storyPath string and copy it
        char* storyPath = strdup(scenarioPath);

        return storyPath;
    }
    else {
        perror("getcwd() error");
        exit(1);
    }
}

void landing()
{
    clearScreen();
    FILE* fp = fopen("ascii/event.txt", "r");
    char* line = readFileContent(fp);

    changeTextColor("yellow");
    printf("%s", line);
    changeTextColor("reset");

    printf("\n");
    printf("Press any key to continue...\n");
    fgetc(stdin);
    clearScreen();
}

int folderExists(const char* path)
{
    if (access(path, F_OK) == 0) {
        // The folder exists
        return 1;
    }
    else {
        // The folder does not exist
        return 0;
    }
}

int hasEventFile(const char* folderPath)
{
    char eventFilePath[MAX_PATH_LENGTH];
    snprintf(eventFilePath, sizeof(eventFilePath), "%s/event.txt", folderPath);

    if (access(eventFilePath, F_OK) == 0) {
        // The event.txt file exists in the folder
        return 0;
    }
    else {
        // The event.txt file does not exist in the folder
        return 1;
    }
}

char* readFile(const char* path, const char* filename)
{
    size_t path_len = strlen(path);
    size_t filename_len = strlen(filename);
    size_t full_path_len = path_len + 1 + filename_len;

    // Allocate memory for the full path
    char* full_path = (char*)malloc(full_path_len + 1);
    if (full_path == NULL) {
        fprintf(stderr, "Memory allocation failed.\n");
        exit(EXIT_FAILURE);
    }

    strcpy(full_path, path);
    strcat(full_path, "/");
    strcat(full_path, filename);

    FILE* file = fopen(full_path, "r");
    if (file == NULL) {
        fprintf(stderr, "Failed to open file: %s\n", full_path);
        free(full_path);
        exit(EXIT_FAILURE);
    }

    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    char* file_content = (char*)malloc(file_size + 1); // +1 for null terminator
    if (file_content == NULL) {
        fprintf(stderr, "Memory allocation failed.\n");
        fclose(file);
        free(full_path);
        exit(EXIT_FAILURE);
    }

    size_t read_size = fread(file_content, 1, file_size, file);
    if (read_size != file_size) {
        fprintf(stderr, "Failed to read file: %s\n", full_path);
        fclose(file);
        free(file_content);
        free(full_path);
        exit(EXIT_FAILURE);
    }

    file_content[file_size] = '\0';

    fclose(file);
    free(full_path);

    return file_content;
}

void event()
{
    landing();

    StoryChoice* storyChoice = malloc(sizeof(StoryChoice));

    int villageID = 0;
    int placeID = 0;
    int scenarioID = 0;

    char* storyPath = initializeStoryChoice(villageID, placeID, scenarioID);
    //printf("storyPath: %s\n", storyPath);

    if (!folderExists(storyPath)) {
        printf("\nThe story path \"%s\" does not exist", storyPath);
        fgetc(stdin);
        return;
    }

    int choice;

    strcpy(storyChoice->storyPath, storyPath);

    while (hasEventFile(storyChoice->storyPath)) {
        printf("storyPath: %s\n", storyChoice->storyPath);

        strcpy(storyChoice->id, readFile(storyChoice->storyPath, "id.txt"));
        strcpy(storyChoice->context, readFile(storyChoice->storyPath, "context.txt"));
        strcpy(storyChoice->situation, readFile(storyChoice->storyPath, "situation.txt"));

        printf("\nID: %s", storyChoice->id);
        printf("\nContext:\n%s\n", storyChoice->context);
        printf("\nSituation:\n%s\n", storyChoice->situation);

        scanf("%d", &choice);

        // Check the user's choice and update the storyPath
        if (choice == 1) {
            strcat(storyChoice->storyPath, "/blue_pill");
        }
        else if (choice == 2) {
            strcat(storyChoice->storyPath, "/red_pill");
        }
        else {
            printf("Invalid choice. Please choose 1 or 2.\n");
            break;
        }
    }

    strcpy(storyChoice->id, readFile(storyChoice->storyPath, "id.txt"));
    strcpy(storyChoice->context, readFile(storyChoice->storyPath, "context.txt"));
    strcpy(storyChoice->situation, readFile(storyChoice->storyPath, "situation.txt"));

    printf("\nID: %s", storyChoice->id);
    printf("\nContext:\n%s\n", storyChoice->context);
    printf("\nSituation:\n%s\n", storyChoice->situation);

    switch (atoi(readFile(storyChoice->storyPath, "event.txt"))) {
    case DEATH:
        changeTextColor("red");
        printf("You died !\n");
        break;
    case BONUS:
        changeTextColor("green");
        printf("You got a bonus !\n");
        break;
    case MALUS:
        changeTextColor("red");
        printf("You got a malus !\n");
        break;
    case REWARD:
        changeTextColor("green");
        printf("You got a reward !\n");
        break;
    case FIGHT:
        changeTextColor("yellow");
        printf("You got a fight !\n");
        break;
    default:
        changeTextColor("blue");
        printf("End of side quest.\n");
        break;
    }
    changeTextColor("reset");
    scanf("%d", &choice);
}
