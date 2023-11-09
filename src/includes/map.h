#ifndef MAP_H
#define MAP_H
#include "start.h"

typedef struct {
    int map_top;
    int map_left;
    int map_width;
    int map_height;
    char *map;
    int player_x;
    int player_y;
} Map;

#define BUFSIZE 32

typedef enum arrowKey_e {
    ARROWKEY_UNKNOWN = 0,
    ARROWKEY_UP,
    ARROWKEY_DOWN,
    ARROWKEY_LEFT,
    ARROWKEY_RIGHT,
} arrowKey_t;

const char *arrowKeyGetName(arrowKey_t arrowKey);
arrowKey_t readArrowKeyPress();
void saveCursorPos();
void restoreCursorPos();
void clearLine();
char signAtCoordinate(const char *map, int x, int y, Map m);
char convertSigntoChar(char sign);
void setSignColor(char sign);
void printSignWithColor(char sign);
void printMapAtCoordinate(int x, int y, char *m);
void movCursor(int x, int y);
void printSignAtCoordinate(char *map, int x, int y, Map *m);
void printPlayerAtCoordinate(int x, int y);
int eventHandler(char sign, Map m, Player *p);
int movRight(int *x, int *y, char *map, Map m, Player *p);
int movLeft(int *x, int *y, char *map, Map m, Player *p);
int movUp(int *x, int *y, char *map, Map m, Player *p);
int movDown(int *x, int *y, char *map, Map m, Player *p);
int mov(Map *m, Player *p);
int map(const char *filename, const char *monster, int map_width, int map_height, int map_left, int map_top, Player *p);
void saveMapToFile(const char *map, const char *filename);

#endif