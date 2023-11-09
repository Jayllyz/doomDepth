#ifndef EVENT_H
#define EVENT_H

typedef struct TreeChoice TreeChoice;
typedef struct Place Place;
typedef struct Village Village;

enum {
    NONE,
    DEATH,
    BONUS,
    MALUS,
    REWARD,
    FIGHT,
};

struct TreeChoice {
    int id;
    char* context;
    char* situation;
    int event;
    TreeChoice* blue_pill;
    TreeChoice* red_pill;
};

typedef struct {
    int id;
    TreeChoice* choice;
} Scenario;

struct Place {
    int id;
    char* name;
    char* description;
    Scenario* scenario;
    Place* next;
};

struct Village {
    int id;
    char* name;
    char* description;
    Scenario* scenario;
    Place* next_place;
    Village* next_village;
};

typedef struct {
    Village* village;
    Place* place;
    Scenario* scenario;
    TreeChoice* choice;
} Protagonist;

void event();
char* initializeStoryChoice(int villageID, int placeID, int scenarioID);
void landing();
int folderExists(const char *path);
int hasEventFile(const char *folderPath);
char* readFile(const char* path, const char* filename);

#endif